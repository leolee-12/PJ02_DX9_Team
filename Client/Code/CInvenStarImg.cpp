#include "pch.h"
#include "CInvenStarImg.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CInvenStarImg::CInvenStarImg(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CInvenStarImg::~CInvenStarImg()
{
}

HRESULT CInvenStarImg::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(1024*m_fScale, 1024*m_fScale, 1.0f);
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);

	m_bRender = true;
	return S_OK;
}


_int CInvenStarImg::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CInvenStarImg::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	m_vWorldPos = m_vParentPos + m_vLocalPos;
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vWorldPos);
	Compute_ViewDepth_Ortho(&m_vWorldPos);
}

void CInvenStarImg::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CInvenStarImg::OnCollision(CGameObject* pObject)
{

}

CInvenStarImg* CInvenStarImg::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale)
{
	CInvenStarImg* pCInvenStarImg = new CInvenStarImg(pGraphicDev);
	pCInvenStarImg->m_vParentPos = _vParentPos;
	pCInvenStarImg->m_vLocalPos = _vLocalPos;
	pCInvenStarImg->m_fScale = _fScale;
	pCInvenStarImg->m_vWorldPos = _vParentPos + _vLocalPos;
	if (FAILED(pCInvenStarImg->Ready_GameObject()))
	{
		Safe_Release(pCInvenStarImg);
		MSG_BOX("pCInvenStarImg Create Failed");
		return nullptr;
	}

	return pCInvenStarImg;
}

HRESULT CInvenStarImg::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InventoryStar"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}





void CInvenStarImg::Free()
{
	CUi::Free();
}
