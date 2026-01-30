#include "pch.h"
#include "CInvenItemInfo.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CItemInfoBg.h"
#include "CInvenSlot.h"
#include "CInvenItem.h"
#include "CInvenEquipItem.h"
//#include "CCookingTargetFood.h"
//#include "CCookingSelectSlot.h"
//#include "CDivider.h"
//#include "CCookingStar.h"
//#include "CCookingInfoRecipe.h"
//#include "CCookingInfoFaith.h"
//#include "CCookingUpDownArrow.h"

#include "CFontMgr.h"
#include "CFontUIOrtho.h"

CInvenItemInfo::CInvenItemInfo(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_bActive(true)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}
CInvenItemInfo::~CInvenItemInfo()
{

}

HRESULT CInvenItemInfo::Ready_GameObject()
{
	m_vPos = _vec3{ WINCX/4,0,0 };
	m_pInfoBg = CItemInfoBg::Create(m_pGraphicDev, _vec3{ 0.0f,0.0f,0.1f }, m_vPos, 0.3f);
	if (nullptr == m_pInfoBg)
		return E_FAIL;

	m_pInfoSlot = CInvenSlot::Create(m_pGraphicDev, _vec3{ 0.0f,50.0f,0.01f }, m_vPos, 0.3f);
	if (nullptr == m_pInfoSlot)
		return E_FAIL;

	m_pInfoDescBg = CInvenSlot::Create(m_pGraphicDev, _vec3{ 0.0f,-70.0f,0.01f }, m_vPos, 1.5f,0.25f);
	if (nullptr == m_pInfoDescBg)
		return E_FAIL;

	m_pInfoItem = CInvenItem::Create(m_pGraphicDev, _vec3{ 0.0f,50.0f,0.001f }, m_vPos, 0.3f, IRT_IMAGE);
	if (nullptr == m_pInfoItem)
		return E_FAIL;
	m_pInfoItem->Set_Render(true);

	m_pEquipItem = CInvenEquipItem::Create(m_pGraphicDev, _vec3{ 0.0f,50.0f,0.001f }, m_vPos, 0.3f);
	if (nullptr == m_pEquipItem)
		return E_FAIL;
	m_pEquipItem->Set_Render(true);

	// 장식
	//_vec3 DividerPos = _vec3(410.f, 140.f, 0.1f);
	//pGameObject = CDivider::Create(m_pGraphicDev, 1, DividerPos, 0.5f);
	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//_vec3 DividerPos1 = _vec3(230.f, 140.f, 0.1f);
	//pGameObject = CDivider::Create(m_pGraphicDev, 0, DividerPos1, 0.5f);
	//if (nullptr == pGameObject)
	//	return E_FAIL;

	m_pItemNameFont = CFontUIOrtho::Create(m_pGraphicDev);
	if (nullptr == m_pItemNameFont)
		return E_FAIL;

	m_pItemFlavorFont = CFontUIOrtho::Create(m_pGraphicDev);
	if (nullptr == m_pItemFlavorFont)
		return E_FAIL;

	m_pItemDescFont = CFontUIOrtho::Create(m_pGraphicDev);
	if (nullptr == m_pItemDescFont)
		return E_FAIL;

	m_pItemNameFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pItemNameFont->Set_FontColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	m_pItemNameFont->Set_Pos(_vec2(WINCX / 4, 0.0f));
	m_pItemNameFont->Set_Scale(_vec2(300.0f, 123.f));
	m_pItemNameFont->Set_Font(L"Font_Default24");
	m_pItemNameFont->Active();

	m_pItemFlavorFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pItemFlavorFont->Set_FontColor(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));
	m_pItemFlavorFont->Set_Pos(_vec2(WINCX / 4, -30.0f));
	m_pItemFlavorFont->Set_Scale(_vec2(300.0f, 120.f));
	m_pItemFlavorFont->Set_Font(L"Font_Default");
	m_pItemFlavorFont->Active();

	m_pItemDescFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pItemDescFont->Set_FontColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f));
	m_pItemDescFont->Set_Pos(_vec2(WINCX / 4, -70.0f));
	m_pItemDescFont->Set_Scale(_vec2(300.0f, 120.f));
	m_pItemDescFont->Set_Font(L"Font_Default");
	m_pItemDescFont->Active();

	Ready_Event();
	return S_OK;
}

_int CInvenItemInfo::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return NOEVENT; }
	
	m_pInfoBg->Update_GameObject(fTimeDelta);
	m_pInfoItem->Update_GameObject(fTimeDelta);
	m_pEquipItem->Update_GameObject(fTimeDelta);
	m_pInfoSlot->Update_GameObject(fTimeDelta);
	m_pInfoDescBg->Update_GameObject(fTimeDelta);
	m_pItemDescFont->Update_GameObject(fTimeDelta);
	m_pItemNameFont->Update_GameObject(fTimeDelta);
	m_pItemFlavorFont->Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return NOEVENT;
}

void CInvenItemInfo::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return; }
	m_pInfoBg->LateUpdate_GameObject(fTimeDelta);
	m_pInfoItem->LateUpdate_GameObject(fTimeDelta);
	m_pEquipItem->LateUpdate_GameObject(fTimeDelta);
	m_pInfoSlot->LateUpdate_GameObject(fTimeDelta);
	m_pInfoDescBg->LateUpdate_GameObject(fTimeDelta);
	m_pItemDescFont->LateUpdate_GameObject(fTimeDelta);
	m_pItemNameFont->LateUpdate_GameObject(fTimeDelta);
	m_pItemFlavorFont->LateUpdate_GameObject(fTimeDelta);
}

void CInvenItemInfo::Render_GameObject()
{
	if (!m_bActive) { return; }

	m_pItemDescFont->Render_GameObject();
	m_pItemNameFont->Render_GameObject();
	m_pItemFlavorFont->Render_GameObject();
}

void CInvenItemInfo::OnCollision(CGameObject* pObject)
{
}


CInvenItemInfo* CInvenItemInfo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CInvenItemInfo* pInvenItemInfo = new CInvenItemInfo(pGraphicDev);

	if (FAILED(pInvenItemInfo->Ready_GameObject()))
	{
		Safe_Release(pInvenItemInfo);
		MSG_BOX("pInvenItemInfo Create Failed");
		return nullptr;
	}

	return pInvenItemInfo;
}

void CInvenItemInfo::Set_ItemDataInit(CItem::ITEMID _itemdID)
{
	if (_itemdID < 0 || ITEM_INDEX_MAX < _itemdID) return;
	ItemTableData itd = ItemTable[_itemdID];

	m_pInfoItem->Set_Render(true);
	m_pEquipItem->Set_Render(false);
	m_pInfoItem->Set_ItemID(_itemdID);
	m_pItemDescFont->Set_Text(itd.Description);
	m_pItemNameFont->Set_Text(itd.Name);
	m_pItemFlavorFont->Set_Text(itd.Flavor);
}

void CInvenItemInfo::Set_ItemDataInit(EquipmentItemID _itemID)
{
	if (_itemID < 0 || EQUIP_ITEM_INDEX_MAX < _itemID) return;
	EquipmentTableData eitd = EquipItemTable[_itemID];

	m_pInfoItem->Set_Render(false);
	m_pEquipItem->Set_Render(true);
	m_pEquipItem->Set_ItemID(_itemID);
	m_pItemDescFont->Set_Text(eitd.Description);
	m_pItemNameFont->Set_Text(eitd.Name);
	m_pItemFlavorFont->Set_Text(eitd.Flavor);
}


void CInvenItemInfo::Free()
{
	Safe_Release(m_pInfoBg);
	Safe_Release(m_pInfoItem);
	Safe_Release(m_pInfoSlot);
	Safe_Release(m_pInfoDescBg);
	Safe_Release(m_pItemDescFont);
	Safe_Release(m_pItemNameFont);
	Safe_Release(m_pItemFlavorFont);
	Safe_Release(m_pEquipItem);
	CUi::Free();
}

void CInvenItemInfo::Ready_Event()
{

}
