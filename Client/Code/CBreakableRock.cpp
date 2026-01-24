#include "pch.h"
#include "CBreakableRock.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CInteractMgr.h"
#include "CFontMgr.h"
#include "CResourceWorkBar.h"
#include <CItem.h>

CBreakableRock::CBreakableRock(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_iTextureIndex(0)
	, m_fScale(1.f)
	, m_fBaseScale(1.f)
	, m_fPhase(0.f)
	, m_fWindSpeed(3.f)
	, m_fWindStrength(0.1f)
	, m_fAccTime(0.f)
	, m_fReactStrength(0.f)
	, m_vReactDir(0.f, 0.f, 0.f)
	, m_fWorkGauge(0.f)
	, m_fPreWorkGauge(0.f)
{
}

CBreakableRock::CBreakableRock(const CBreakableRock& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_iTextureIndex(rhs.m_iTextureIndex)
	, m_fScale(rhs.m_fScale)
	, m_fBaseScale(rhs.m_fBaseScale)
	, m_fPhase(0.f)
	, m_fWindSpeed(3.f)
	, m_fWindStrength(0.1f)
	, m_fAccTime(0.f)
	, m_fReactStrength(0.f)
	, m_vReactDir(0.f, 0.f, 0.f)
	, m_fWorkGauge(0.f)
	, m_fPreWorkGauge(0.f)
{
}

CBreakableRock::~CBreakableRock()
{
}

HRESULT CBreakableRock::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pColliderCom->RegisterToManager(this, CL_GRASS);
	CInteractMgr::GetInstance()->Register_IObj(CInteractMgr::ROCK, this);

	m_iTextureIndex = 0;
	m_pWorkBar = CResourceWorkBar::Create(m_pGraphicDev, _float(m_iHp), _vec3{});
	m_pWorkBar->UnActive();

	return S_OK;
}

_int CBreakableRock::Update_GameObject(const _float& fTimeDelta)
{
	if (g_bDebug) { m_pColliderCom->Update_AABBforRender(); }

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	Update_WorkBar(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		CInteractMgr::GetInstance()->Unregister_IObj(CInteractMgr::ROCK, this);
		Create_Item();
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CBreakableRock::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!(m_fWorkGauge - m_fPreWorkGauge < 0.0001f))
	{
		m_pWorkBar->Active();
	}

	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Compute_Bilboard(BBD_X);
	Compute_ViewDepth(&vPos);

	Check_Status();

	m_pWorkBar->LateUpdate_GameObject(fTimeDelta);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBreakableRock::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture(m_iTextureIndex);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

_vec3* CBreakableRock::Get_WorkPos(_vec3* pWorkPos) const
{
	m_pTransformCom->Get_Info(INFO_POS, pWorkPos);
	return pWorkPos;
}

void CBreakableRock::Set_ObjectData(const Engine::OBJECTDATA& objData)
{
	m_iTextureIndex = objData.textureIndex;
	m_fScale = objData.scale;
	m_fBaseScale = objData.scale;

	m_pTransformCom->Set_Pos(objData.x, objData.y, objData.z);
	m_pTransformCom->Set_Scale(m_fScale, m_fScale, m_fScale);
}

void CBreakableRock::Check_Status()
{
	if (Is_WorkComplete()) m_iHp = 0;

	m_fPreWorkGauge = m_fWorkGauge;
}

void CBreakableRock::OnCollision(CGameObject* pObject)
{
}

void CBreakableRock::Update_WorkBar(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	vPos.y += 3.f;
	m_pWorkBar->Set_TargetPos(vPos);
	m_pWorkBar->Update_CurWork(m_fWorkGauge);
	m_pWorkBar->Update_GameObject(fTimeDelta);
}

void CBreakableRock::Create_Item()
{
	_int itemCount = Get_Rand_Int(3, 5);
	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	for (_uint i = 0; i < itemCount; ++i)
	{
		CGameObject* pItem;
		_float fY(vPos.y - m_pTransformCom->Get_Scale(ROT_Y) * 0.25f);
		pItem = CItem::Create(m_pGraphicDev, m_pMessageChannel, _vec3(vPos.x, fY, vPos.z), CItem::IG_STONE, true);

		if (pItem)
		{
			wstring strObjTag = L"Item";

			IMessageChannel::EVENT ESummonMonster;
			ESummonMonster.strType = L"Obj.Add";
			ESummonMonster.eOBJID = Engine::OID_ITEM;
			ESummonMonster.hmapData.emplace(L"Obj", pItem);
			ESummonMonster.hmapData.emplace(L"LayerTag", L"GameLogic_Layer");
			ESummonMonster.hmapData.emplace(L"ObjTag", strObjTag);
			m_pMessageChannel->Publish(ESummonMonster);
		}
	}
}

HRESULT CBreakableRock::Add_Component()
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

	// Grass Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BreakableStoneTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));
	//static_cast<Engine::CCollider*>(pComponent)->Set_AABB();

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

CBreakableRock* CBreakableRock::Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::OBJECTDATA& objData, IMessageChannel* pMessageChannel)
{
	CBreakableRock* pBreakableRock = new CBreakableRock(pGraphicDev);

	pBreakableRock->m_pMessageChannel = pMessageChannel;
	pBreakableRock->m_pMessageChannel->AddRef();

	if (FAILED(pBreakableRock->Ready_GameObject()))
	{
		Safe_Release(pBreakableRock);
		MSG_BOX("pBreakableRock Create Failed");
		return nullptr;
	}

	pBreakableRock->Set_ObjectData(objData);
	pBreakableRock->m_pTransformCom->Update_Component(0.f);

	return pBreakableRock;
}

void CBreakableRock::Free()
{
	Safe_Release(m_pWorkBar);
	CGameObject::Free();
}
