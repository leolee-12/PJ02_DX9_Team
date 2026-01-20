#include "pch.h"
#include "CBuilding.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CInteractMgr.h"
#include "CFontMgr.h"

CBuilding::CBuilding(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_eBuildingType(BT_END)
	, m_eBuildingState(BS_END)
	, m_fWorkGauge(0.f)
{
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
{
}

CBuilding::~CBuilding()
{
}

HRESULT CBuilding::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();

	return S_OK;
}

_int CBuilding::Update_GameObject(const _float& fTimeDelta)
{
	if (g_bDebug) { m_pColliderCom->Update_AABBforRender(); }

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

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
	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	if (m_eBuildingState == BS_COMPLETE) m_pTransformCom->Compute_Bilboard(BBD_X);

	Compute_ViewDepth(&vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
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

	_matrix matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	_vec3 vWorldPos, vViewPos, vndcPos, vScreenPos;
	m_pTransformCom->Get_Info(INFO_POS, &vWorldPos);
	D3DXVec3TransformCoord(&vViewPos, &vWorldPos, &matView);
	D3DXVec3TransformCoord(&vndcPos, &vViewPos, &matProj);

	vScreenPos.x = (vndcPos.x * 0.5f + 0.5f) * _float(WINCX);
	vScreenPos.y = (-vndcPos.y * 0.5f + 0.5f) * _float(WINCY);

	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	wchar_t szGauge[16];

	swprintf_s(szGauge, L" Gauge : %.3f", m_fWorkGauge);
	RECT rcPlayer = { vScreenPos.x - 50, vScreenPos.y - 10, vScreenPos.x + 50, vScreenPos.y + 10 };
	CFontMgr::GetInstance()->Render_Font(L"Font_Lapture20", szGauge, rcPlayer, FontColor, DT_RIGHT | DT_BOTTOM);
}

void CBuilding::OnCollision(CGameObject* pObject)
{
}

void CBuilding::Add_WorkGauge(_float fWork)
{
	if (m_eBuildingState != BS_CONSTRUCTING) return;

	m_fWorkGauge += fWork;

	if (m_fWorkGauge >= MAX_WORK_GAUGE)
	{
		Change_State(BS_COMPLETE);
	}
}

wstring CBuilding::Get_CompleteTexKey()
{
	switch (m_eBuildingType)
	{
	case BT_WORKSHOP:		return L"Proto_Building_Workshop";
	case BT_COOK:			return L"Proto_Building_Cook";
	case BT_KNUCKLEBONE:	return L"Proto_Building_Knucklebone";
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
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));
	//static_cast<Engine::CCollider*>(pComponent)->Set_AABB();

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

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

		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		m_pTransformCom->Set_Pos(vPos.x, m_fGroundY, vPos.z);
		m_pTransformCom->Rotation(ROT_X, 0.f);
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

	m_pColliderCom->RegisterToManager(this, CL_GRASS);
}

CBuilding* CBuilding::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, const _vec3& vPos, BUILDING_TYPE eType)
{
	CBuilding* pBuilding = new CBuilding(pGraphicDev);

	pBuilding->m_pMessageChannel = pMessageChannel;
	pBuilding->m_pMessageChannel->AddRef();
	pBuilding->m_eBuildingType = eType;

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
	CGameObject::Free();
}
