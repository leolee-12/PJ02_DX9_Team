#include "pch.h"
#include "CInteractionUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CInteractionUI::CInteractionUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CInteractionUI::~CInteractionUI()
{
}

HRESULT CInteractionUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale((900.f * 0.2f), (374.f * 0.2f), 1.f);
	m_pTransformCom->Set_Pos(0.f, -250.f, 0.2f);

	return S_OK;
}

_int CInteractionUI::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return NOEVENT; }

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CInteractionUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return; }

	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CInteractionUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CInteractionUI::OnCollision(CGameObject* pObject)
{

}

HRESULT CInteractionUI::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InteractionUI"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CInteractionUI* CInteractionUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CInteractionUI* pInteractionUI = new CInteractionUI(pGraphicDev);

	if (FAILED(pInteractionUI->Ready_GameObject()))
	{
		Safe_Release(pInteractionUI);
		MSG_BOX("pInteractionUI Create Failed");
		return nullptr;
	}

	return pInteractionUI;
}

void CInteractionUI::Free()
{
	CUi::Free();
}
