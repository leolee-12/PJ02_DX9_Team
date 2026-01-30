#include "pch.h"
#include "CItemInfoBg.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CItemInfoBg::CItemInfoBg(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CItemInfoBg::~CItemInfoBg()
{
}

HRESULT CItemInfoBg::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(1450 * m_fScale, 800 * m_fScale, 0.3f);
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);

	m_bRender = true;
	return S_OK;
}


_int CItemInfoBg::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CItemInfoBg::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	m_vWorldPos = m_vParentPos + m_vLocalPos;
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vWorldPos);
	Compute_ViewDepth_Ortho(&m_vWorldPos);
}

void CItemInfoBg::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CItemInfoBg::OnCollision(CGameObject* pObject)
{

}

CItemInfoBg* CItemInfoBg::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale)
{
	CItemInfoBg* pCItemInfoBg = new CItemInfoBg(pGraphicDev);
	pCItemInfoBg->m_vParentPos = _vParentPos;
	pCItemInfoBg->m_vLocalPos = _vLocalPos;
	pCItemInfoBg->m_fScale = _fScale;
	pCItemInfoBg->m_vWorldPos = _vParentPos + _vLocalPos;
	if (FAILED(pCItemInfoBg->Ready_GameObject()))
	{
		Safe_Release(pCItemInfoBg);
		MSG_BOX("pCItemInfoBg Create Failed");
		return nullptr;
	}

	return pCItemInfoBg;
}

HRESULT CItemInfoBg::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InventoryItemInfoBack"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}





void CItemInfoBg::Free()
{
	CUi::Free();
}
