#include "pch.h"
#include "CInvenEquipSlot.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CInvenEquipSlot::CInvenEquipSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CInvenEquipSlot::~CInvenEquipSlot()
{
}

HRESULT CInvenEquipSlot::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(512.0f * m_fWidthScale, 512.0f * m_fHeightScale, 1.0f);
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	m_bRender = true;
	return S_OK;
}

_int CInvenEquipSlot::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CInvenEquipSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }

	m_vWorldPos = m_vParentPos + m_vLocalPos;
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	Compute_ViewDepth_Ortho(&m_vWorldPos);
}

void CInvenEquipSlot::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CInvenEquipSlot::OnCollision(CGameObject* pObject)
{
}

HRESULT CInvenEquipSlot::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InventoryRhombus"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CInvenEquipSlot* CInvenEquipSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale)
{
	CInvenEquipSlot* pCInvenEquipSlot = new CInvenEquipSlot(pGraphicDev);
	pCInvenEquipSlot->m_vLocalPos = _vLocalPos;
	pCInvenEquipSlot->m_vParentPos = _vParentPos;
	pCInvenEquipSlot->m_fWidthScale = _fScale;
	pCInvenEquipSlot->m_fHeightScale = _fScale;
	pCInvenEquipSlot->m_vWorldPos = _vParentPos + _vLocalPos;

	if (FAILED(pCInvenEquipSlot->Ready_GameObject()))
	{
		Safe_Release(pCInvenEquipSlot);
		MSG_BOX("pCInvenEquipSlot Create Failed");
		return nullptr;
	}

	return pCInvenEquipSlot;
}

CInvenEquipSlot* CInvenEquipSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _Width, _float _Height)
{
	CInvenEquipSlot* pCInvenEquipSlot = new CInvenEquipSlot(pGraphicDev);
	pCInvenEquipSlot->m_vLocalPos = _vLocalPos;
	pCInvenEquipSlot->m_vParentPos = _vParentPos;
	pCInvenEquipSlot->m_fWidthScale = _Width;
	pCInvenEquipSlot->m_fHeightScale = _Height;
	pCInvenEquipSlot->m_vWorldPos = _vParentPos + _vLocalPos;

	if (FAILED(pCInvenEquipSlot->Ready_GameObject()))
	{
		Safe_Release(pCInvenEquipSlot);
		MSG_BOX("pCInvenEquipSlot Create Failed");
		return nullptr;
	}

	return pCInvenEquipSlot;
}

void CInvenEquipSlot::Free()
{
	CUi::Free();
}
