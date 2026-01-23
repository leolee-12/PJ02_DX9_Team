#include "pch.h"
#include "CResourceHistoryBack.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CResourceHistoryBack::CResourceHistoryBack(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CResourceHistoryBack::~CResourceHistoryBack()
{
}

HRESULT CResourceHistoryBack::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(550.f * m_fScale * 2.0f, 123.f * m_fScale*1.1f, 0.1f);
	m_pTransformCom->Set_Pos(-WINCX / 2 + m_vPos.x, -m_vPos.y + WINCY / 2, m_vPos.z);

	return S_OK;
}

_int CResourceHistoryBack::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CResourceHistoryBack::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Set_Pos(-WINCX / 2 + m_vPos.x, -m_vPos.y + WINCY / 2, m_vPos.z);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CResourceHistoryBack::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pTextureCom->Set_Texture();


	m_pBufferCom->Render_Buffer();
}

void CResourceHistoryBack::OnCollision(CGameObject* pObject)
{

}

HRESULT CResourceHistoryBack::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ItemHistoryBack"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


CResourceHistoryBack* CResourceHistoryBack::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale)
{
	CResourceHistoryBack* pCResourceHistoryBack = new CResourceHistoryBack(pGraphicDev);

	pCResourceHistoryBack->m_vPos = _vPos;
	pCResourceHistoryBack->m_fScale = _fScale;


	if (FAILED(pCResourceHistoryBack->Ready_GameObject()))
	{
		Safe_Release(pCResourceHistoryBack);
		MSG_BOX("pCResourceHistoryBack Create Failed");
		return nullptr;
	}
	return pCResourceHistoryBack;
}

void CResourceHistoryBack::Free()
{
	CUi::Free();
}
