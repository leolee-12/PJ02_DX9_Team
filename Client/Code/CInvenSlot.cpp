#include "pch.h"
#include "CInvenSlot.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CInvenSlot::CInvenSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CInvenSlot::~CInvenSlot()
{
}

HRESULT CInvenSlot::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(232 * m_fScale, 232 * m_fScale, 0.f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	m_bRender = true;
	return S_OK;
}

_int CInvenSlot::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CInvenSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CInvenSlot::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CInvenSlot::OnCollision(CGameObject* pObject)
{

}

HRESULT CInvenSlot::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InventorySlot"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CInvenSlot* CInvenSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fScale)
{
	CInvenSlot* pCInvenSlot = new CInvenSlot(pGraphicDev);
	pCInvenSlot->m_vPos = vPos;
	pCInvenSlot->m_fScale = fScale;


	if (FAILED(pCInvenSlot->Ready_GameObject()))
	{
		Safe_Release(pCInvenSlot);
		MSG_BOX("pCInvenSlot Create Failed");
		return nullptr;
	}

	return pCInvenSlot;
}

void CInvenSlot::Free()
{
	CUi::Free();
}
