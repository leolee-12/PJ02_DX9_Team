#include "pch.h"
#include "CPlayerWeaponUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CPlayerWeaponUI::CPlayerWeaponUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CPlayerWeaponUI::~CPlayerWeaponUI()
{
}

HRESULT CPlayerWeaponUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(154.0f * m_fScale, 154.0f * m_fScale, 0.f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, 0.02f);

	m_bRender = true;
	Ready_Event();
	return S_OK;
}

_int CPlayerWeaponUI::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CPlayerWeaponUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CPlayerWeaponUI::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pTextureCom->Set_Texture(m_iPage);


	m_pBufferCom->Render_Buffer();
}

void CPlayerWeaponUI::OnCollision(CGameObject* pObject)
{

}

HRESULT CPlayerWeaponUI::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlayerWeaponUI"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CPlayerWeaponUI* CPlayerWeaponUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, _float _fScale, _int _iPage)
{
	CPlayerWeaponUI* pPlayerWeaponUI = new CPlayerWeaponUI(pGraphicDev);

	pPlayerWeaponUI->m_iPage = _iPage;
	pPlayerWeaponUI->m_vPos = _vPos;
	pPlayerWeaponUI->m_fScale = _fScale;

	if (FAILED(pPlayerWeaponUI->Ready_GameObject()))
	{
		Safe_Release(pPlayerWeaponUI);
		MSG_BOX("pPlayerWeaponUI Create Failed");
		return nullptr;
	}

	return pPlayerWeaponUI;
}

void CPlayerWeaponUI::Free()
{
	CUi::Free();
}

void CPlayerWeaponUI::Ready_Event()
{
}
