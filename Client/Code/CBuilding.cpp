#include "pch.h"
#include "CBuilding.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CInteractMgr.h"
#include "CFontMgr.h"
#include "CTriggerPoint.h"
#include "CItem.h"
#include "CResourceWorkBar.h"
#include "CPersistentMgr.h"
#include "CShrineSpot.h"
#include "CSoundMgr.h"
#include "CCutSceneMgr.h"
#include "CRatau.h"

CBuilding::CBuilding(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_eBuildingType(BT_END)
	, m_eBuildingState(BS_END)
	, m_fWorkGauge(0.f)
	, m_fPreWorkGauge(0.f)
	, m_pTrigger(nullptr)
	, m_pWorkBar(nullptr)
{
	m_vecSubObjects.reserve(6);
}

CBuilding::CBuilding(const CBuilding& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_eBuildingType(rhs.m_eBuildingType)
	, m_eBuildingState(rhs.m_eBuildingState)
	, m_fWorkGauge(0.f)
	, m_fPreWorkGauge(0.f)
	, m_pTrigger(nullptr)
	, m_pWorkBar(nullptr)
{
	m_vecSubObjects.reserve(6);
}

CBuilding::~CBuilding()
{
}

HRESULT CBuilding::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();

	Ready_Event();

	if (FAILED(Ready_PixelShader()))
		return E_FAIL;

	return S_OK;
}

_int CBuilding::Update_GameObject(const _float& fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	for(size_t i = 0; i < m_vecSubObjects.size(); ++i)
	{
		if (m_vecSubObjects[i]) m_vecSubObjects[i]->Update_GameObject(fTimeDelta);
	}

	if (m_eBuildingState != BS_COMPLETE) 		Update_WorkBar(fTimeDelta);
	else if (m_bUsingTrigger)					m_pTrigger->Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();

		if(m_eBuildingState == BS_CONSTRUCTING) CInteractMgr::GetInstance()->Unregister_IObj(CInteractMgr::BUILD, this);
	}

	if(m_eBuildingState == BS_CONSTRUCTING) CRenderer::GetInstance()->Add_RenderGroup(RENDER_TILE, this);
	else									CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CBuilding::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_fWorkGauge - m_fPreWorkGauge > 0.0001f)
	{
		m_pWorkBar->Active();

		if (m_fAccTime >= 0.5f)
		{
			//_uint iChannel = Get_Rand_Int(SOUND_EFFECT1, SOUND_EFFECT10);

			_tchar strSoundName[128] = L"";
			swprintf_s(strSoundName, L"Hammering_%d.wav", Get_Rand_Int(0, 33));
			CSoundMgr::GetInstance()->Play(strSoundName, SOUND_BUILD, 0.5f);

			m_fAccTime = 0.f;
		}
	}
	else
	{
		if (m_fAccTime >= 3.f)
		{
			m_pWorkBar->UnActive();
			m_fAccTime = 0.f;
		}
	}

	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	switch (m_eBuildingState)
	{
	case BS_PREVIEW:
		m_pTransformCom->Compute_Bilboard(BBD_X);

		Compute_ViewDepth(&vPos);
		break;
	case BS_CONSTRUCTING:
		m_fPreWorkGauge = m_fWorkGauge;
		break;
	case BS_COMPLETE:
		if (m_bUsingTrigger) { m_pTrigger->LateUpdate_GameObject(fTimeDelta); }

		m_pTransformCom->Compute_Bilboard(BBD_X);

		Compute_ViewDepth(&vPos);
		break;
	}

	m_pWorkBar->LateUpdate_GameObject(fTimeDelta);
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	for (size_t i = 0; i < m_vecSubObjects.size(); ++i)
	{
		if (m_vecSubObjects[i]) m_vecSubObjects[i]->LateUpdate_GameObject(fTimeDelta);
	}

	_vec3 vColliderPos = m_vPos;
	vColliderPos.y -= 1.f;
	_vec3 vTriggetHalfSize = { 1.99f, 2.f, 1.99f };
	AABB tAABB = { vColliderPos, vTriggetHalfSize };
	m_pColliderCom->Set_AABB(tAABB);
	m_pColliderCom->UpdateFromCustom(tAABB);
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();

	if (m_eBuildingState == BS_PREVIEW) { m_bCanPlace = true; }
}

void CBuilding::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Alpha test (remove fully transparent pixels)
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x10);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	if (m_eBuildingState == BS_PREVIEW)
	{
		_float fCanPlace = m_bCanPlace ? 1.f : 0.f;
		m_pGraphicDev->SetPixelShader(m_pPixelShader);
		m_pGraphicDev->SetPixelShaderConstantF(0, &fCanPlace, 1);
	}

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	if (m_eBuildingState == BS_PREVIEW)
	{
		m_pGraphicDev->SetPixelShader(NULL);
	}

	// Restore
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CBuilding::OnCollision(CGameObject* pObject)
{
	if (pObject->Get_OBJID() == OID_BUILD || pObject->Get_OBJID() == OID_BREAK)
	{
		m_bCanPlace = false;
	}
}

void CBuilding::Add_WorkGauge(_float fWork)
{
	if (m_eBuildingState == BS_COMPLETE) return;

	m_fWorkGauge += fWork;

	if (m_fWorkGauge >= MAX_WORK_GAUGE)
	{
		m_pWorkBar->UnActive();
		Change_State(BS_COMPLETE);
	}
}

_vec3* CBuilding::Get_WorkPos(_vec3* pWorkPos) const
{
	m_pTransformCom->Get_Info(INFO_POS, pWorkPos);
	return pWorkPos;
}

wstring CBuilding::Get_CompleteTexKey()
{
	switch (m_eBuildingType)
	{
	case BT_WORKSHOP:		return L"Proto_Building_Workshop";
	case BT_COOK:			return L"Proto_Building_Cook";
	case BT_KNUCKLEBONE:	return L"Proto_Building_Knucklebone";
	case BT_SHRINE:			return L"Proto_Building_Shrine";
	default:				return L"Proto_Building_Default";
	}
}

void CBuilding::Set_PosForPick(const _vec3& vPos)
{
	m_vPos = vPos;
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pTransformCom->Update_Component(0.f);
}

void CBuilding::Set_Placement()
{
	Change_State(BS_CONSTRUCTING);
}

HRESULT CBuilding::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// GrassBuffer (dynamic vertex buffer for sway effect)
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Building_Constructing"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Construct", pComponent });

	// Texture
	pComponent = Engine::CProtoMgr::GetInstance()->Clone_Prototype(Get_CompleteTexKey());

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Complete", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

HRESULT CBuilding::Ready_PixelShader()
{
	LPD3DXBUFFER pCode = NULL;
	LPD3DXBUFFER pError = NULL;

	// HLSL 파일 컴파일 
	HRESULT hr = D3DXCompileShaderFromFile(
		L"../Shader/Building.hlsl", // 파일명 
		NULL, // 매크로 
		NULL, // include 
		"PS_Preview", // 엔트리 포인트 
		"ps_2_0", // 셰이더 모델 
		0, // 플래그 
		&pCode,
		&pError,
		NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(NULL,
				(char*)pError->GetBufferPointer(),
				"Shader Error",
				MB_OK);
			pError->Release();
		}
		return E_FAIL;
	} // 픽셀 셰이더 생성 

	if (pCode) {
		m_pGraphicDev->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &m_pPixelShader);
		pCode->Release();
	}

	if (pError) {
		pError->Release();
	}

	return S_OK;
}

void CBuilding::Change_State(BUILDING_STATE eState)
{
	if (m_eBuildingState == eState) return;

	// Exit_State
	switch (m_eBuildingState)
	{
	case BS_CONSTRUCTING:
		CInteractMgr::GetInstance()->Unregister_IObj(CInteractMgr::BUILD, this);
		break;
	}

	m_eBuildingState = eState;

	_float fScale = 5.f;
	// Enter_State
	switch (eState)
	{
	case BS_PREVIEW:
		m_pTextureCom = static_cast<CTexture*>(Get_Component(ID_STATIC, L"Com_Texture_Complete"));
		m_fGroundY = DEFAULT_COMPLETE_GROUNDY;
		m_fWorkGauge = 0.f;

		switch (m_eBuildingType)
		{
		case BT_WORKSHOP:
			fScale = 7.f;
			m_fGroundY -= fScale * 0.1f;
			break;
		case BT_SHRINE:
		{
			fScale = 10.f;
			m_fGroundY += fScale * 0.2f;

			CGameObject* pGameObject = nullptr;
			_vec3 vPos = m_vPos;
			_float fRadius = 5.f;
			_float fRadian = 0.f;
		}
		break;
		default:
			break;
		}

		break;
	case BS_CONSTRUCTING:
		m_pTextureCom = static_cast<CTexture*>(Get_Component(ID_STATIC, L"Com_Texture_Construct"));
		CInteractMgr::GetInstance()->Register_IObj(CInteractMgr::BUILD, this);
		m_pTransformCom->Rotation(ROT_X, 90.f);
		m_fGroundY = DEFAULT_CONSTRUCT_GROUNDY;
		m_fWorkGauge = 0.f;
		break;

	case BS_COMPLETE:
	{
		m_pTextureCom = static_cast<CTexture*>(Get_Component(ID_STATIC, L"Com_Texture_Complete"));
		m_fGroundY = DEFAULT_COMPLETE_GROUNDY;
		m_fPreWorkGauge = m_fWorkGauge = 1.f;
		Ready_Trigger();

		

		switch (m_eBuildingType)
		{
		case BT_WORKSHOP:
			fScale = 7.f;
			m_fGroundY -= fScale * 0.1f;
			break;

		case BT_SHRINE:
		{
			fScale = 10.f;
			m_fGroundY += fScale * 0.2f;

			CGameObject* pGameObject = nullptr;
			_vec3 vPos = m_vPos;
			_float fRadius = 5.f;
			_float fRadian = 0.f;

			for (size_t i = 0; i < 6; ++i)
			{
				vPos.x = m_vPos.x + fRadius * cosf(fRadian);
				vPos.z = m_vPos.z + fRadius * sinf(fRadian);

				pGameObject = CShrineSpot::Create(m_pGraphicDev, vPos, m_pMessageChannel);
				NULL_CHECK(pGameObject);
				m_vecSubObjects.push_back(pGameObject);
				fRadian += D3DXToRadian(60.f);
			}
		}
			break;

		default:
			break;
		}

	}
		break;
	}

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Set_Pos(vPos.x, m_fGroundY, vPos.z);
	m_pTransformCom->Rotation(ROT_X, 0.f);
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);
}

void CBuilding::Player_Interact()
{
}

void CBuilding::Set_Texture()
{

}

void CBuilding::Ready_Variable()
{
	_float fScale = 5.f;
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);
	m_fAccTime = 0.f;

	m_eOBJID = OID_BUILD;

	m_pColliderCom->RegisterToManager(this, CL_BUILD);
	m_pWorkBar = CResourceWorkBar::Create(m_pGraphicDev, _float(m_iHp), _vec3{});
	m_pWorkBar->UnActive();
}

void CBuilding::Ready_Event()
{
	switch (m_eBuildingType)
	{
	case BT_DUMMY:
		break;
	case BT_WORKSHOP:
		break;
	case BT_COOK:
		m_hmapSubHandles.insert({ L"Cooking.End", m_pMessageChannel->Subscribe(L"Cooking.End", [this](const IMessageChannel::EVENT& Event) {
		{
				auto iter = Event.hmapData.find(L"isSuccess");
				if (iter == Event.hmapData.end()) { return; }

				_bool isSuccess = any_cast<_bool>(iter->second);

			_vec3 vPos;
			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			vPos.y -= 1.f;

			CGameObject* pItem = nullptr;

			if (isSuccess)
			{
				pItem = CItem::Create(m_pGraphicDev, m_pMessageChannel, vPos, CItem::FD_GFOOD, false, 3.f);
			}
			else
			{
				pItem = CItem::Create(m_pGraphicDev, m_pMessageChannel, vPos, CItem::FD_BFOOD, false, 3.f);
			}

			IMessageChannel::EVENT CookingEvent;
			CookingEvent.strType = L"Obj.Add";
			CookingEvent.hmapData[L"Obj"] = pItem;
			CookingEvent.hmapData[L"LayerTag"] = L"GameLogic_Layer";
			CookingEvent.hmapData[L"ObjTag"] = wstring(L"Item");
			m_pMessageChannel->Publish(CookingEvent);
		}
		}) });
		break;
	case BT_KNUCKLEBONE:
		m_hmapSubHandles.insert({ L"Trigger.Activate", m_pMessageChannel->Subscribe(L"Trigger.Activate", [this](const IMessageChannel::EVENT& Event)
		{
			auto Owneriter = Event.hmapData.find(L"Trigger_TID");
			if (Owneriter == Event.hmapData.end()) { return; }

			if (any_cast<Trigger::TRIGGERID>(Owneriter->second) == Trigger::TI_KNUCKLE)
			{
				_vec3 vCutScenePos = { m_vPos.x, m_vPos.y, m_vPos.z - 5.f };

				CGameObject* pRatau = CRatau::Create(m_pGraphicDev, m_pMessageChannel, vCutScenePos);

				IMessageChannel::EVENT eAddRatau;
				eAddRatau.strType = L"Obj.Add";
				eAddRatau.eOBJID = Engine::OID_MONSTER;
				eAddRatau.hmapData.emplace(L"Obj", pRatau);
				eAddRatau.hmapData[L"LayerTag"] = L"GameLogic_Layer";
				eAddRatau.hmapData.emplace(L"ObjTag", wstring(L"Ratau"));
				m_pMessageChannel->Publish(eAddRatau);

				CUTSCENE tKnuckleBoneScene;
				tKnuckleBoneScene.strName = L"Meet_Knucklebone";
				tKnuckleBoneScene.vecSteps =
				{
					{ vCutScenePos, 1.f, 1.f, L"Player", L"LookforCam", ADV_IMMEDIATE},
					{ vCutScenePos, 0.75f, 0.5f, L"Ratau", L"Ratau_Intro", ADV_EVENT, 0.f, L"Ratau.Done" },
					{ vCutScenePos, 1.5f, 0.5f, L"Ratau", L"너클본에 흥미가 있어보이는구려." },
					{ vCutScenePos, 1.5f, 0.5f, L"Ratau", L"자, 나랑 너클본 한 판 하시겠소?", ADV_DIALOGUE, 0.f, L"", vector<wstring>({L"예.", L"아니오."}) },
					{ m_vPos, 1.f, 0.5f, L"FadeOut", L"", ADV_TIMED, 2.f},
					{ m_vPos, 1.f, 0.5f, L"Ratau", L"Destroy", ADV_IMMEDIATE},
				};

				CCutSceneMgr::GetInstance()->Register_CutScene(tKnuckleBoneScene);

				IMessageChannel::EVENT tBuildingEvent;
				tBuildingEvent.strType = L"Staging.Start";
				tBuildingEvent.hmapData[L"StagingName"] = wstring(L"Meet_Knucklebone");
				m_pMessageChannel->Publish(tBuildingEvent);
			}
		}
	) });
		break;
	}
}

void CBuilding::Ready_Trigger()
{
	_vec3 vTriggerPos = m_vPos;
	vTriggerPos.y -= 1.f;
	_vec3 vTriggetHalfSize = { 2.f,2.f,2.f };

	switch (m_eBuildingType)
	{
	case BT_DUMMY:
		break;
	case BT_WORKSHOP:
		m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggetHalfSize, Trigger::TI_CRAFTING, L"Crafting");
		m_bUsingTrigger = true;
		break;
	case BT_COOK:
		m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggetHalfSize, Trigger::TI_COOKING, L"Cooking");
		m_bUsingTrigger = true;
		break;
	case BT_KNUCKLEBONE:
		m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggetHalfSize, Trigger::TI_KNUCKLE, L"KnuckleBone");
		m_bUsingTrigger = true;
		break;
	case BT_SHRINE:
		//m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggetHalfSize, Trigger::TI_CRAFTING, L"Crafting");
		m_bUsingTrigger = false;
		break;
	}
}

void CBuilding::Update_WorkBar(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	vPos.y += 3.f;
	m_pWorkBar->Set_TargetPos(vPos);
	m_pWorkBar->Update_CurWork(m_fWorkGauge);
	m_pWorkBar->Update_GameObject(fTimeDelta);
}

void CBuilding::PrepareDestroy()
{
	m_pColliderCom->UnregisterFromManager();
	if (m_bUsingTrigger && m_pTrigger != nullptr)
	{
		Safe_Destroy(m_pTrigger);
	}
}

CBuilding* CBuilding::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, const _vec3& vPos, BUILDING_TYPE eType, BUILDING_STATE eState)
{
	CBuilding* pBuilding = new CBuilding(pGraphicDev);

	pBuilding->m_pMessageChannel = pMessageChannel;
	pBuilding->m_pMessageChannel->AddRef();
	pBuilding->m_eBuildingType = eType;
	pBuilding->m_vPos = vPos;

	if (FAILED(pBuilding->Ready_GameObject()))
	{
		Safe_Release(pBuilding);
		MSG_BOX("pBuilding Create Failed");
		return nullptr;
	}

	if (pBuilding->m_eBuildingType == BT_WORKSHOP)
	{
		pBuilding->Change_State(BS_COMPLETE);
	}
	else
	{
		pBuilding->Change_State(eState);
	}

	pBuilding->m_pTransformCom->Set_Pos(vPos.x, pBuilding->m_fGroundY, vPos.z);
	pBuilding->m_pTransformCom->Update_Component(0.f);

	return pBuilding;
}

void CBuilding::Free()
{
	for(size_t i = 0; i < m_vecSubObjects.size(); ++i)
	{
		Safe_Destroy(m_vecSubObjects[i]);
	}

	Safe_Destroy(m_pTrigger);
	Safe_Release(m_pWorkBar);
	CGameObject::Free();
}
