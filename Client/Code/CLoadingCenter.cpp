#include "pch.h"
#include "CLoadingCenter.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CLoadingCenter::CLoadingCenter(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CLoadingCenter::~CLoadingCenter()
{
}

HRESULT CLoadingCenter::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(250.f, 250.f, 1.f);
	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);

	return S_OK;
}

_int CLoadingCenter::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CLoadingCenter::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CLoadingCenter::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
}

void CLoadingCenter::OnCollision(CGameObject* pObject)
{

}

HRESULT CLoadingCenter::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LoadingCenterTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CLoadingCenter* CLoadingCenter::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLoadingCenter* pLodingBack = new CLoadingCenter(pGraphicDev);

	if (FAILED(pLodingBack->Ready_GameObject()))
	{
		Safe_Release(pLodingBack);
		MSG_BOX("pLodingBack Create Failed");
		return nullptr;
	}

	return pLodingBack;
}

void CLoadingCenter::Free()
{
	CUi::Free();
}