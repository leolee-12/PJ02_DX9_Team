#include "pch.h"
#include "CInvenItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"


CInvenItem::CInvenItem(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CInvenItem::~CInvenItem()
{
}

HRESULT CInvenItem::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(114 * m_fScale, 93 * m_fScale, 0.2f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	m_bRender = true;
	m_iCount = 0;
	return S_OK;
}

HRESULT CInvenItem::Ready_Material()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

_int CInvenItem::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return iExit;
}

void CInvenItem::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CInvenItem::Render_GameObject()
{
	if (!m_bRender) { return; }

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CInvenItem::OnCollision(CGameObject* pObject)
{

}

HRESULT CInvenItem::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InventoryItem"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


	return S_OK;
}



CInvenItem* CInvenItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fScale)
{
	CInvenItem* pCInvenItem = new CInvenItem(pGraphicDev);

	pCInvenItem->m_vPos = vPos;
	pCInvenItem->m_fScale = fScale;


	if (FAILED(pCInvenItem->Ready_GameObject()))
	{
		Safe_Release(pCInvenItem);
		MSG_BOX("pCInvenItem Create Failed");
		return nullptr;
	}

	return pCInvenItem;
}

void CInvenItem::Free()
{
	CUi::Free();
}
