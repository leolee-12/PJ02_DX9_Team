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

CBuilding::CBuilding(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	//, m_pColliderCom(nullptr)
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
	//, m_pColliderCom(nullptr)
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

	return S_OK;
}

_int CBuilding::Update_GameObject(const _float& fTimeDelta)
{
	//if (g_bDebug) { m_pColliderCom->Update_AABBforRender(); }

	//m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	for(size_t i = 0; i < m_vecSubObjects.size(); ++i)
	{
		if (m_vecSubObjects[i]) m_vecSubObjects[i]->Update_GameObject(fTimeDelta);
	}

	if (m_eBuildingState == BS_CONSTRUCTING) 	Update_WorkBar(fTimeDelta);
	else										m_pTrigger->Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		//m_pColliderCom->UnregisterFromManager();

		if(m_eBuildingState == BS_CONSTRUCTING) CInteractMgr::GetInstance()->Unregister_IObj(CInteractMgr::BUILD, this);
	}

	if(m_eBuildingState == BS_CONSTRUCTING) CRenderer::GetInstance()->Add_RenderGroup(RENDER_TILE, this);
	else									CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CBuilding::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!(m_fWorkGauge - m_fPreWorkGauge < 0.0001f))
	{
		m_pWorkBar->Active();
	}

	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	if (m_eBuildingState == BS_COMPLETE)
	{
		m_pTrigger->LateUpdate_GameObject(fTimeDelta);

		//m_pTransformCom->Compute_Bilboard(BBD_X);

		Compute_ViewDepth(&vPos);
	}
	else if (m_eBuildingState == BS_CONSTRUCTING) m_fPreWorkGauge = m_fWorkGauge;

	m_pWorkBar->LateUpdate_GameObject(fTimeDelta);
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	for (size_t i = 0; i < m_vecSubObjects.size(); ++i)
	{
		if (m_vecSubObjects[i]) m_vecSubObjects[i]->LateUpdate_GameObject(fTimeDelta);
	}
}

void CBuilding::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Alpha test (remove fully transparent pixels)
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x10);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	// Restore
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CBuilding::OnCollision(CGameObject* pObject)
{
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
	//pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
	//	(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));
	////static_cast<Engine::CCollider*>(pComponent)->Set_AABB();

	//if (nullptr == pComponent)
	//	return E_FAIL;

	//m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

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

	// Enter_State
	switch (eState)
	{
	case BS_CONSTRUCTING:
		CInteractMgr::GetInstance()->Register_IObj(CInteractMgr::BUILD, this);
		m_pTransformCom->Rotation(ROT_X, 90.f);
		m_fGroundY = DEFAULT_CONSTRUCT_GROUNDY;
		break;

	case BS_COMPLETE:
	{
		m_pTextureCom = static_cast<CTexture*>(Get_Component(ID_STATIC, L"Com_Texture_Complete"));
		m_fGroundY = DEFAULT_COMPLETE_GROUNDY;

		_float fScale = 5.f;

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

		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		m_pTransformCom->Set_Pos(vPos.x, m_fGroundY, vPos.z);
		m_pTransformCom->Rotation(ROT_X, 0.f);
		m_pTransformCom->Set_Scale(fScale, fScale, fScale);

		Ready_Trigger();
	}
		break;
	}
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

	//Change_State(BS_CONSTRUCTING);
	//m_fWorkGauge = 0.f;

	// 테스트용
	m_fWorkGauge = 1.f;
	Change_State(BS_COMPLETE);
	// 테스트용

	if(m_eBuildingType == BT_WORKSHOP)
	{
		m_fWorkGauge = 1.f;
		Change_State(BS_COMPLETE);
	}

	//m_pColliderCom->RegisterToManager(this, CL_GRASS);
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
		break;
	case BT_COOK:
		m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggetHalfSize, Trigger::TI_COOKING, L"Cooking");
		break;
	case BT_KNUCKLEBONE:
		m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggetHalfSize, Trigger::TI_KNUCKLE, L"KnuckleBone");
		break;
	case BT_SHRINE:
		m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggetHalfSize, Trigger::TI_CRAFTING, L"Crafting");
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

CBuilding* CBuilding::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, const _vec3& vPos, BUILDING_TYPE eType)
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

	pBuilding->m_pTransformCom->Set_Pos(vPos.x, pBuilding->m_fGroundY, vPos.z);
	pBuilding->m_pTransformCom->Update_Component(0.f);

	return pBuilding;
}

void CBuilding::Free()
{
	for(size_t i = 0; i < m_vecSubObjects.size(); ++i)
	{
		Safe_Release(m_vecSubObjects[i]);
	}

	Safe_Release(m_pTrigger);
	Safe_Release(m_pWorkBar);
	CGameObject::Free();
}
