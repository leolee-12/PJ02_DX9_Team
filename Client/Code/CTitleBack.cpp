#include "pch.h"
#include "CTitleBack.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CTitleBack::CTitleBack(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CTitleBack::~CTitleBack()
{
}

HRESULT CTitleBack::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale((_float)WINCX, (_float)WINCY, 1.f);
	m_pTransformCom->Set_Pos(0.f, 0.f, 1.f);

	return S_OK;
}

_int CTitleBack::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CTitleBack::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CTitleBack::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetTexture(0, NULL);

	m_pBufferCom->Render_Buffer();
}

void CTitleBack::OnCollision(CGameObject* pObject)
{

}

HRESULT CTitleBack::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcColTitle*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcColTitle"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}



CTitleBack* CTitleBack::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTitleBack* pTitleBack = new CTitleBack(pGraphicDev);

	if (FAILED(pTitleBack->Ready_GameObject()))
	{
		Safe_Release(pTitleBack);
		MSG_BOX("pLodingBack Create Failed");
		return nullptr;
	}

	return pTitleBack;
}

void CTitleBack::Free()
{
	CUi::Free();
}