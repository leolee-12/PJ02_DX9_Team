#include "pch.h"
#include "CResourceHistoryItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CResourceHistoryItem::CResourceHistoryItem(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CResourceHistoryItem::~CResourceHistoryItem()
{
}

HRESULT CResourceHistoryItem::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(128.f * m_fScale, 128.f * m_fScale, 0.1f);
	m_pTransformCom->Set_Pos(-WINCX / 2 + m_vPos.x - 60, -m_vPos.y + WINCY / 2, m_vPos.z);

	return S_OK;
}

_int CResourceHistoryItem::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CResourceHistoryItem::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Set_Pos(-WINCX / 2 + m_vPos.x - 60, -m_vPos.y + WINCY / 2, m_vPos.z);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CResourceHistoryItem::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iPage);


	m_pBufferCom->Render_Buffer();
}

void CResourceHistoryItem::OnCollision(CGameObject* pObject)
{

}

HRESULT CResourceHistoryItem::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ItemHistoryItem"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


CResourceHistoryItem* CResourceHistoryItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale, _int _iPage)
{
	CResourceHistoryItem* pCResourceHistoryItem = new CResourceHistoryItem(pGraphicDev);

	pCResourceHistoryItem->m_vPos = _vPos;
	pCResourceHistoryItem->m_fScale = _fScale;
	pCResourceHistoryItem->m_iPage = _iPage;


	if (FAILED(pCResourceHistoryItem->Ready_GameObject()))
	{
		Safe_Release(pCResourceHistoryItem);
		MSG_BOX("pCResourceHistoryItem Create Failed");
		return nullptr;
	}
	return pCResourceHistoryItem;
}

void CResourceHistoryItem::Free()
{
	CUi::Free();
}
