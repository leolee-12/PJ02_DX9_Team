#include "pch.h"
#include "CResourceHistoryDeco.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CResourceHistoryDeco::CResourceHistoryDeco(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CResourceHistoryDeco::~CResourceHistoryDeco()
{
}

HRESULT CResourceHistoryDeco::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(514.f * m_fScale * 2.0f, 102.f * m_fScale*1.1f, 0.1f);
	m_pTransformCom->Set_Pos(-WINCX / 2 + m_vPos.x, -m_vPos.y + WINCY / 2, m_vPos.z);

	return S_OK;
}

_int CResourceHistoryDeco::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CResourceHistoryDeco::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Set_Pos(-WINCX / 2 + m_vPos.x, -m_vPos.y + WINCY / 2, m_vPos.z);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CResourceHistoryDeco::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CResourceHistoryDeco::OnCollision(CGameObject* pObject)
{

}

HRESULT CResourceHistoryDeco::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ItemHistoryDeco"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


CResourceHistoryDeco* CResourceHistoryDeco::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale)
{
	CResourceHistoryDeco* pCResourceHistoryDeco = new CResourceHistoryDeco(pGraphicDev);

	pCResourceHistoryDeco->m_vPos = _vPos;
	pCResourceHistoryDeco->m_fScale = _fScale;


	if (FAILED(pCResourceHistoryDeco->Ready_GameObject()))
	{
		Safe_Release(pCResourceHistoryDeco);
		MSG_BOX("pCResourceHistoryDeco Create Failed");
		return nullptr;
	}
	return pCResourceHistoryDeco;
}

void CResourceHistoryDeco::Free()
{
	CUi::Free();
}
