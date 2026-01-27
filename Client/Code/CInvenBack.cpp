#include "pch.h"
#include "CInvenBack.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CInvenBack::CInvenBack(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CInvenBack::~CInvenBack()
{
}

HRESULT CInvenBack::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_vWorldPos = m_vParentPos + m_vLocalPos;
	m_pTransformCom->Set_Scale(WINCX / 2, WINCY, 1.0f);
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);

	m_bRender = true;
	return S_OK;
}


_int CInvenBack::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CInvenBack::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	m_vWorldPos = m_vParentPos + m_vLocalPos;
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vWorldPos);
	Compute_ViewDepth_Ortho(&m_vWorldPos);
}

void CInvenBack::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CInvenBack::OnCollision(CGameObject* pObject)
{

}

CInvenBack* CInvenBack::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos,_vec3 _vParentPos, _float _fScale)
{
	CInvenBack* pCInvenBack = new CInvenBack(pGraphicDev);
	pCInvenBack->m_vParentPos = _vParentPos;
	pCInvenBack->m_vLocalPos = _vLocalPos;
	pCInvenBack->m_fScale = _fScale;

	if (FAILED(pCInvenBack->Ready_GameObject()))
	{
		Safe_Release(pCInvenBack);
		MSG_BOX("pCInvenBack Create Failed");
		return nullptr;
	}

	return pCInvenBack;
}

HRESULT CInvenBack::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InventoryBack"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}





void CInvenBack::Free()
{
	CUi::Free();
}
