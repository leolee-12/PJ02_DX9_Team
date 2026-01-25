#include "pch.h"
#include "CWeaponInfo.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CWeaponInfo::CWeaponInfo(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CWeaponInfo::~CWeaponInfo()
{
}

HRESULT CWeaponInfo::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale((420.f * 0.9f), (200.f * 0.9f), 1.f);
	m_pTransformCom->Set_Pos(0.f, -175.f, 0.3f);

	if (m_eType >= WINFO_END || m_eType < WINFO_SWORD)
	{
		MSG_BOX("웨폰인포 인덱스 범위초과");
		return E_FAIL;
	}

	return S_OK;
}

_int CWeaponInfo::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return NOEVENT; }

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CWeaponInfo::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return; }

	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CWeaponInfo::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(_uint(m_eType));

	m_pBufferCom->Render_Buffer();
}

void CWeaponInfo::OnCollision(CGameObject* pObject)
{

}

HRESULT CWeaponInfo::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WeaponInfo"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CWeaponInfo* CWeaponInfo::Create(LPDIRECT3DDEVICE9 pGraphicDev, WINFOTYPE eType)
{
	CWeaponInfo* pWeaponInfo = new CWeaponInfo(pGraphicDev);

	pWeaponInfo->m_eType = eType;

	if (FAILED(pWeaponInfo->Ready_GameObject()))
	{
		Safe_Release(pWeaponInfo);
		MSG_BOX("pWeaponInfo Create Failed");
		return nullptr;
	}

	return pWeaponInfo;
}

void CWeaponInfo::Free()
{
	CUi::Free();
}
