#include "pch.h"
#include "CInvenEquipItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CFontUIOrtho.h"

CInvenEquipItem::CInvenEquipItem(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_bMsgRegistered(false)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CInvenEquipItem::~CInvenEquipItem()
{
}

HRESULT CInvenEquipItem::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	m_bRender = true;
	m_bHover = false;


	m_vScreenPos = _vec2(WINCX / 2 + m_vWorldPos.x, WINCY / 2 - m_vWorldPos.y);
	m_vHitHalfScale = _vec2((128.0f * m_fScale) / 2, (128.0f * m_fScale) / 2);
	m_pTransformCom->Set_Scale(128.0f * m_fScale, 128.0f * m_fScale, 1.0f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	m_tItemData.ID = EID_NONE;
	return S_OK;
}

_int CInvenEquipItem::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	if (m_tItemData.bOnHoverEvet) { OnHover_CusorColl(); }
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return iExit;
}

void CInvenEquipItem::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }

	m_vWorldPos = m_vParentPos + m_vLocalPos;
	m_vScreenPos = _vec2(WINCX / 2 + m_vWorldPos.x, WINCY / 2 - m_vWorldPos.y);
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vWorldPos);
	Compute_ViewDepth_Ortho(&m_vWorldPos);
}

void CInvenEquipItem::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (m_tItemData.ID < 1 || EQUIP_ITEM_INDEX_MAX < m_tItemData.ID)
		m_iPage = 0;
	else
		m_iPage = m_tItemData.ID;
	m_pTextureCom->Set_Texture(m_iPage);

	m_pBufferCom->Render_Buffer();
}

void CInvenEquipItem::OnCollision(CGameObject* pObject)
{

}

void CInvenEquipItem::OnHover_CusorColl()
{
	//if (!m_bRender) { return; }

	POINT pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	RECT tRc = {
		_long(m_vScreenPos.x - m_vHitHalfScale.x),
		_long(m_vScreenPos.y - m_vHitHalfScale.y),
		_long(m_vScreenPos.x + m_vHitHalfScale.x),
		_long(m_vScreenPos.y + m_vHitHalfScale.y)
	};


	if (PtInRect(&tRc, pt))
	{
		m_bHover = true;
		IMessageChannel::EVENT OnHoverEvent;
		OnHoverEvent.strType = L"CInvenEquipItem.OnHover";
		OnHoverEvent.hmapData[L"ItemID"] = m_tItemData.ID;
		m_pMessageChannel->Publish(OnHoverEvent);

		//if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		//{
		//	// 클릭이벤트
		//}

	}
	else {
		if (m_bHover)
		{
			IMessageChannel::EVENT OnHoverExitEvent;
			OnHoverExitEvent.strType = L"CInvenEquipItem.OnHoverExit";
			m_pMessageChannel->Publish(OnHoverExitEvent);
			m_bHover = false;
		}
	}
}

HRESULT CInvenEquipItem::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InventoryEquipItem"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


	return S_OK;
}
void CInvenEquipItem::Ready_Event()
{
	if (m_pMessageChannel == nullptr) return;
	if (m_bMsgRegistered) return;
	m_bMsgRegistered = true;
}

void CInvenEquipItem::Set_MessageChannel(IMessageChannel* pMessageChannel)
{

	if (pMessageChannel == nullptr) { return; }
	if (m_pMessageChannel) {
		CGameObject::Unsubscribe_Handles();
		Safe_Release(m_pMessageChannel);
		m_bMsgRegistered = false;
	}
	m_pMessageChannel = pMessageChannel;
	m_pMessageChannel->AddRef();
	Ready_Event();
}



CInvenEquipItem* CInvenEquipItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale)
{
	CInvenEquipItem* pCInvenEquipItem = new CInvenEquipItem(pGraphicDev);

	pCInvenEquipItem->m_vLocalPos = _vLocalPos;
	pCInvenEquipItem->m_vParentPos = _vParentPos;
	pCInvenEquipItem->m_fScale = _fScale;
	pCInvenEquipItem->m_vWorldPos = _vParentPos + _vLocalPos;

	if (FAILED(pCInvenEquipItem->Ready_GameObject()))
	{
		Safe_Release(pCInvenEquipItem);
		MSG_BOX("pCInvenEquipItem Create Failed");
		return nullptr;
	}

	return pCInvenEquipItem;
}

void CInvenEquipItem::Free()
{
	CUi::Free();
}
