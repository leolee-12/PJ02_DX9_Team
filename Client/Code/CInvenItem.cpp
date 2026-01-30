#include "pch.h"
#include "CInvenItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CFontUIOrtho.h"

#include "CItemDataTable.h"
CInvenItem::CInvenItem(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_bMsgRegistered(false)
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

	m_pCountFont = CFontUIOrtho::Create(m_pGraphicDev);


	m_pCountFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pCountFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pCountFont->Set_Pos(_vec2(m_vWorldPos.x+20.0f, m_vWorldPos.y-20.0f));
	m_pCountFont->Set_Scale(_vec2(59.f * 2.f, 123.f * 0.5f));
	m_pCountFont->Set_Font(L"Font_Default24");
	m_pCountFont->Set_Text(to_wstring(m_tItemData.iCount));
	m_pCountFont->Active();
	m_bRender = false;
	m_bHover = false;


	m_vScreenPos = _vec2(WINCX / 2 + m_vWorldPos.x, WINCY / 2 - m_vWorldPos.y);
	m_vHitHalfScale = _vec2((128.0f * m_fScale) / 2, (128.0f * m_fScale) / 2);
	m_pTransformCom->Set_Scale(128.0f * m_fScale, 128.0f * m_fScale, 1.0f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
	return S_OK;
}

_int CInvenItem::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	if (m_tItemData.bOnHoverEvet) { OnHover_CusorColl(); }
	if (m_tItemData.RenderType == IRT_COUNT)
	{
		m_pCountFont->Set_Pos(_vec2(m_vWorldPos.x + 20.0f, m_vWorldPos.y - 20.0f));
		m_pCountFont->Update_GameObject(fTimeDelta);
	}
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return iExit;
}

void CInvenItem::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	if (m_tItemData.RenderType == IRT_COUNT)
		m_pCountFont->Set_Text(to_wstring(m_tItemData.iCount));

	m_vWorldPos = m_vParentPos + m_vLocalPos;
	m_vScreenPos = _vec2(WINCX / 2 + m_vWorldPos.x, WINCY / 2 - m_vWorldPos.y);
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vWorldPos);
	Compute_ViewDepth_Ortho(&m_vWorldPos);
}

void CInvenItem::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (m_tItemData.ID < 0 || ITEM_INDEX_MAX < m_tItemData.ID)
		m_iPage = 0;
	else
		m_iPage = m_tItemData.ID;
	m_pTextureCom->Set_Texture(m_iPage);

	m_pBufferCom->Render_Buffer();
	if (m_tItemData.RenderType == IRT_COUNT)
		m_pCountFont->Render_GameObject();
}

void CInvenItem::OnCollision(CGameObject* pObject)
{

}

void CInvenItem::OnHover_CusorColl()
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
		OnHoverEvent.strType = L"CInvenItem.OnHover";
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
			OnHoverExitEvent.strType = L"CInvenItem.OnHoverExit";
			m_pMessageChannel->Publish(OnHoverExitEvent);
			m_bHover = false;
		}
	}
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
void CInvenItem::Ready_Event()
{
	if (m_pMessageChannel == nullptr) return;
	if (m_bMsgRegistered) return;
	m_bMsgRegistered = true;
}

void CInvenItem::Set_MessageChannel(IMessageChannel* pMessageChannel)
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



CInvenItem* CInvenItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale, ITEM_RENDER_TYPE _RenderType)
{
	CInvenItem* pCInvenItem = new CInvenItem(pGraphicDev);

	pCInvenItem->m_vLocalPos = _vLocalPos;
	pCInvenItem->m_vParentPos = _vParentPos;
	pCInvenItem->m_fScale = _fScale;
	pCInvenItem->m_vWorldPos = _vParentPos + _vLocalPos;
	pCInvenItem->m_tItemData.RenderType = _RenderType;

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
	Safe_Release(m_pCountFont);
	CUi::Free();
}
