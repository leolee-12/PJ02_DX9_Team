#include "pch.h"
#include "CInventory.h"

#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"
#include "CPersistentMgr.h"

#include "CInvenBack.h"
#include "CInvenSlot.h"
#include "CInvenItem.h"
#include "CInvenBtn.h"
#include "CInvenItemInfo.h"

#include "CInvenStarImg.h"
#include "CInvenRedFog.h"
#include "CFontUIOrtho.h"
#include "CInvenPlayer.h"
#include "CInvenPlayerHp.h"
#include "CInvenEquipItem.h"

#include "Building_Enum.h"

CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_bMsgRegistered(false)
{
}

CInventory::~CInventory()
{

}

HRESULT CInventory::Ready_GameObject()
{
	m_vPos = _vec3{ -700,0,0 };
	m_pInvenBack = CInvenBack::Create(m_pGraphicDev,_vec3(-WINCX / 4,0.0f,0.01f), m_vPos,1.0f);
	if (nullptr == m_pInvenBack)	
		return E_FAIL;

	m_pItemInvenBtn = CInvenBtn::Create(m_pGraphicDev, _vec3(-480.0f, 190.0f, 0.f), m_vPos, 0.4f);
	if (nullptr == m_pItemInvenBtn)
		return E_FAIL;
	m_pItemInvenBtn->Set_Tex(L"소모품");

	m_pPlayerInfoBtn = CInvenBtn::Create(m_pGraphicDev, _vec3(-360.0f, 190.0f, 0.f), m_vPos, 0.4f);
	if (nullptr == m_pPlayerInfoBtn)
		return E_FAIL;
	m_pPlayerInfoBtn->Set_Tex(L"정보창");


	CInvenSlot* pSlot;
	CInvenItem* pItem;
	CInvenEquipItem* pEquipItem;

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{

			pSlot = CInvenSlot::Create(m_pGraphicDev, { -520.f + (75.f * j), 80.f - (75.f * i), 0.01f }, m_vPos, 0.3f);
			pItem = CInvenItem::Create(m_pGraphicDev, { -520.f + (75.f * j), 80.f - (75.f * i), 0.01f }, m_vPos, 0.3f,IRT_COUNT);
			pItem->Set_HoverEvent(true);
			if (nullptr == pSlot)
				return E_FAIL;

			m_vSlotLocalPos.push_back(_vec3{ -520.f + (75.f * j), 80.f - (75.f * i), 0.01f });
			m_vSlot.push_back(pSlot);
			m_vItem.push_back(pItem);

		}
	}

	_float angle = -150.0f;
	_float length = 170.0f;
	for (int i = 0; i < 10; ++i)
	{
		_vec2 v2 = _vec2(cosf(D3DXToRadian(angle)), sinf(D3DXToRadian(angle))) * length;

		pSlot = CInvenSlot::Create(m_pGraphicDev, { -WINCX / 4 + (v2.x), -50.f - (v2.y), 0.01f }, m_vPos, 0.25f);
		pEquipItem = CInvenEquipItem::Create(m_pGraphicDev, { -WINCX / 4 + (v2.x), -50.f - (v2.y), 0.01f }, m_vPos, 0.2f);
		pEquipItem->Set_HoverEvent(true);
		pSlot->Set_Page(1);
		if (nullptr == pSlot)
			return E_FAIL;
		m_vPlayerSlot.push_back(pSlot);
		m_vPlayerEquipItem.push_back(pEquipItem);
		angle -= 27.0f;
	}

	m_pInfoStarImg = CInvenStarImg::Create(m_pGraphicDev, _vec3(-WINCX / 4, -50.0f, 0.01f), m_vPos, 0.2f);
	if (nullptr == m_pInfoStarImg)
		return E_FAIL;

	m_pInfoRedFog = CInvenRedFog::Create(m_pGraphicDev, _vec3(-WINCX / 4, -50.0f, 0.001f), m_vPos, 1.0f);
	if (nullptr == m_pInfoRedFog)
		return E_FAIL;

	m_pInvenPlyer = CInvenPlayer::Create(m_pGraphicDev, _vec3(-WINCX / 4, -20.0f, 0.0001f), m_vPos, 0.7f);
	if (nullptr == m_pInvenPlyer)
		return E_FAIL;

	m_pInvenPlayerHp = CInvenPlayerHp::Create(m_pGraphicDev, _vec3(-WINCX / 4 -50.0f, 100.0f, 0.01f), m_vPos, 0.3f);
	if (nullptr == m_pInvenPlayerHp)
		return E_FAIL;

	m_pPlayerNameFont = CFontUIOrtho::Create(m_pGraphicDev);
	if (nullptr == m_pPlayerNameFont)
		return E_FAIL;

	m_pPlayerNameFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pPlayerNameFont->Set_FontColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	m_pPlayerNameFont->Set_Pos(_vec2(-WINCX / 4+m_vPos.x, -30.0f + m_vPos.y));
	m_pPlayerNameFont->Set_Scale(_vec2(300.0f, 123.f));
	m_pPlayerNameFont->Set_Font(L"Font_Default30_Heavy");
	m_pPlayerNameFont->Set_Text(L"어린 양");
	m_pPlayerNameFont->Active();

	m_pResourceFont = CFontUIOrtho::Create(m_pGraphicDev);
	if (nullptr == m_pResourceFont)
		return E_FAIL;

	m_pResourceFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pResourceFont->Set_FontColor(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f));
	m_pResourceFont->Set_Pos(_vec2(-WINCX / 4 + m_vPos.x, -30.0f + m_vPos.y));
	m_pResourceFont->Set_Scale(_vec2(200.0f, 123.f));
	m_pResourceFont->Set_Font(L"Font_Default24");
	m_pResourceFont->Set_Text(L"재화");
	m_pResourceFont->Active();

	m_pInvenItemInfo = CInvenItemInfo::Create(m_pGraphicDev);
	m_pInvenItemInfo->Set_Active(false);
	m_eCurInvenState = INVEN_END;
	m_ePreInvenState = INVEN_END;
	m_eInvenRenderState = INVENRT_RESOURCE;
	m_fLerpTime = 1.0f;
	m_vLerpStart = { 0,0,0 };
	m_vLerpEnd = { 0,0,0 };
	m_iCurItemCount = 0;
	//Add_Item(CItem::IG_GOLD, 17);
	//Add_Item(CItem::IG_WOOD, 18);
	//Add_Item(CItem::IG_STONE, 19);
	//Add_Item(CItem::IG_BERRY, 20);
	return S_OK;
}

_int CInventory::Update_GameObject(const _float& fTimeDelta)
{
	Key_Input_Inven();
	if (!m_bActive) { return NOEVENT; }


	if (m_eCurInvenState != m_ePreInvenState)
	{
		switch (m_eCurInvenState)
		{
		case INVEN_IDLE:
		{
			m_bActive = false;
			break;
		}
		case INVEN_OPEN:
		{
			Moveto(m_vPos, _vec3{ -700,0,0 });
			m_bActive = true;
			break;
		}
		case INVEN_STAY:
		{
			break;
		}
		case INVEN_CLOSE:
		{
			Moveto(m_vPos, _vec3{ 0,0,0 });
			break;
		}
		case INVEN_END:
			break;
		default:
			break;
		}
			m_ePreInvenState = m_eCurInvenState;
	}
	if (m_eCurInvenState == INVEN_OPEN || m_eCurInvenState == INVEN_CLOSE )
	{
		if (m_fLerpTime < 1.0f)
		{
			m_fLerpTime += fTimeDelta * 3.0f;
			D3DXVec3Lerp(&m_vPos, &m_vLerpStart, &m_vLerpEnd, m_fLerpTime);
		}
		else
		{
			m_fLerpTime = 1.0f;
			D3DXVec3Lerp(&m_vPos, &m_vLerpStart, &m_vLerpEnd, m_fLerpTime);

			// 수정 예정
			//if (m_eCurInvenState == IS_OPEN)
			//{
			//	Set_State(IS_STAY);
			//}
			//else if(m_eCurInvenState == IS_CLOSE)
			//{
			//	Set_State(IS_IDLE);
			//}
		}
	}

	// 인벤 디폴트 객체
	m_pPlayerInfoBtn->Set_ParentPos(m_vPos);
	m_pItemInvenBtn->Set_ParentPos(m_vPos);
	m_pInvenBack->Set_ParentPos(m_vPos);
	m_pPlayerInfoBtn->Update_GameObject(fTimeDelta);
	m_pItemInvenBtn->Update_GameObject(fTimeDelta);
	m_pInvenBack->Update_GameObject(fTimeDelta);

	m_pInvenItemInfo->Update_GameObject(fTimeDelta);

	// 0번이 무기칸
	CPlayer* pPlayer = CPersistentMgr::GetInstance()->Get_Player();
	if (!pPlayer->Get_Village())
	{
		if (pPlayer->Get_Weapon() == CPlayer::WEAPONTYPE::WT_SWORD)
		{
			m_vPlayerEquipItem[0]->Set_ItemID(EID_SWORD);
		}
		if (pPlayer->Get_Weapon() == CPlayer::WEAPONTYPE::WT_GAUNTLETS)
		{
			m_vPlayerEquipItem[0]->Set_ItemID(EID_GAUNTLETS);
		}
	}
	else
	{
		m_vPlayerEquipItem[0]->Set_ItemID(EID_NONE);
	}

	switch (m_eInvenRenderState)
	{
	case INVENRT_RESOURCE:
	{
		// 소모품창
		for (auto it : m_vSlot)
		{
			it->Set_ParentPos(m_vPos);
			it->Update_GameObject(fTimeDelta);
		}
		for (auto it : m_vItem)
		{
			it->Set_ParentPos(m_vPos);
			it->Update_GameObject(fTimeDelta);
		}
		m_pResourceFont->Set_Pos(_vec2(-540.0f + m_vPos.x, 130.0f + m_vPos.y));
		m_pResourceFont->Update_GameObject(fTimeDelta);
		break;
	}
	case INVENRT_PLAYERINFO:
	{
		// 정보창
		m_pInfoRedFog->Set_ParentPos(m_vPos);
		m_pInfoStarImg->Set_ParentPos(m_vPos);
		m_pPlayerNameFont->Set_Pos(_vec2(-WINCX/4 + m_vPos.x, 150.0f + m_vPos.y));
		m_pInvenPlyer->Set_ParentPos(m_vPos);
		m_pInvenPlayerHp->Set_ParentPos(m_vPos);
		m_pInfoRedFog->Update_GameObject(fTimeDelta);
		m_pInfoStarImg->Update_GameObject(fTimeDelta);
		m_pPlayerNameFont->Update_GameObject(fTimeDelta);
		m_pInvenPlyer->Update_GameObject(fTimeDelta);
		m_pInvenPlayerHp->Update_GameObject(fTimeDelta);
		for (auto it : m_vPlayerSlot)
		{
			it->Set_ParentPos(m_vPos);
			it->Update_GameObject(fTimeDelta);
		}
		for (auto it : m_vPlayerEquipItem)
		{
			it->Set_ParentPos(m_vPos);
			it->Update_GameObject(fTimeDelta);
		}
		break;
	}
	case INVENRT_END:
		break;
	default:
		break;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return NOEVENT;
}

void CInventory::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return; }
	m_pPlayerInfoBtn->LateUpdate_GameObject(fTimeDelta);
	m_pItemInvenBtn->LateUpdate_GameObject(fTimeDelta);
	m_pInvenBack->LateUpdate_GameObject(fTimeDelta);
	m_pInvenItemInfo->LateUpdate_GameObject(fTimeDelta);

	if (m_pPlayerInfoBtn->Get_OnClick())
	{
		m_eInvenRenderState = INVENRT_PLAYERINFO;
		m_pPlayerInfoBtn->Set_OnClick(false);
	}
	if (m_pItemInvenBtn->Get_OnClick())
	{
		m_eInvenRenderState = INVENRT_RESOURCE;
		m_pItemInvenBtn->Set_OnClick(false);
	}


	switch (m_eInvenRenderState)
	{
	case INVENRT_RESOURCE:
	{
		//소모품창
		for (auto it : m_vSlot)
		{
			it->LateUpdate_GameObject(fTimeDelta);
		}
		for (auto it : m_vItem)
		{
			it->LateUpdate_GameObject(fTimeDelta);
		}
		m_pResourceFont->LateUpdate_GameObject(fTimeDelta);
		break;
	}
	case INVENRT_PLAYERINFO:
	{
		// 정보창
		m_pInfoRedFog->LateUpdate_GameObject(fTimeDelta);
		m_pInfoStarImg->LateUpdate_GameObject(fTimeDelta);
		m_pPlayerNameFont->LateUpdate_GameObject(fTimeDelta);
		m_pInvenPlyer->LateUpdate_GameObject(fTimeDelta);
		m_pInvenPlayerHp->LateUpdate_GameObject(fTimeDelta);
		for (auto it : m_vPlayerSlot)
		{
			it->LateUpdate_GameObject(fTimeDelta);
		}
		for (auto it : m_vPlayerEquipItem)
		{
			it->LateUpdate_GameObject(fTimeDelta);
		}
		break;
	}
	case INVENRT_END:
		break;
	default:
		break;
	}
}

void CInventory::Render_GameObject()
{
	if (!m_bActive) { return; }

	switch (m_eInvenRenderState)
	{
	case INVENRT_RESOURCE:
	{
		//소모품창
		m_pResourceFont->Render_GameObject();
		break;
	}
	case INVENRT_PLAYERINFO:
	{
		// 정보창
		m_pInvenPlayerHp->Render_GameObject();
		m_pPlayerNameFont->Render_GameObject();
		break;
	}
	case INVENRT_END:
		break;
	default:
		break;
	}
}

void CInventory::Key_Input_Inven()
{
	// 디버기용 인풋
	if (CDInputMgr::GetInstance()->Key_Down(DIK_TAB))
	{	
		if (m_eCurInvenState == INVEN_IDLE || m_eCurInvenState == INVEN_CLOSE)
			Set_State(INVEN_OPEN);
		else
			Set_State(INVEN_CLOSE);
	}
	// 삭제 테스트용 디버깅
	if (CDInputMgr::GetInstance()->Key_Down(DIK_1))
	{
		Use_Item(CItem::IG_GOLD, 1);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_2))
	{
		Use_Item(CItem::IG_BERRY, 1);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_3))
	{
		Use_Item(CItem::IG_STONE, 1);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_4))
	{
		Use_Item(CItem::IG_WOOD, 1);
	}
}

void CInventory::Set_MessageChannel(IMessageChannel* pMessageChannel)
{

	if (pMessageChannel == nullptr) { return; }
	if (m_pMessageChannel) {
		CGameObject::Unsubscribe_Handles();
		Safe_Release(m_pMessageChannel);
		m_bMsgRegistered = false;
	}
	m_pMessageChannel = pMessageChannel;
	m_pMessageChannel->AddRef();

	
	for (auto it : m_vItem)
	{
		it->Set_MessageChannel(pMessageChannel);
	}

	for (auto it : m_vPlayerEquipItem)
	{
		it->Set_MessageChannel(pMessageChannel);
	}
	Ready_Event();
}

void CInventory::Ready_Event()
{
	if (m_pMessageChannel == nullptr) return;
	if (m_bMsgRegistered) return;

	m_hmapSubHandles.insert({ L"CInventory.ItemInfoRender",m_pMessageChannel->Subscribe(L"CInvenItem.OnHover",[this](const IMessageChannel::EVENT& Event)
		{
			auto iter = Event.hmapData.find(L"ItemID");
			if (iter == Event.hmapData.end())
				return;
		
			CItem::ITEMID ItemID = any_cast<CItem::ITEMID>(iter->second);
			m_pInvenItemInfo->Set_ItemDataInit(ItemID);
			m_pInvenItemInfo->Set_Active(true);
		}
	) });

	m_hmapSubHandles.insert({ L"CInventory.ItemInfoUnRender",m_pMessageChannel->Subscribe(L"CInvenItem.OnHoverExit",[this](const IMessageChannel::EVENT& Event)
		{
			m_pInvenItemInfo->Set_Active(false);
		}
	) });

	m_hmapSubHandles.insert({ L"CInventory.EquipItemInfoRender",m_pMessageChannel->Subscribe(L"CInvenEquipItem.OnHover",[this](const IMessageChannel::EVENT& Event)
		{
			auto iter = Event.hmapData.find(L"ItemID");
			if (iter == Event.hmapData.end())
				return;
		
			EquipmentItemID ItemID = any_cast<EquipmentItemID>(iter->second);
			m_pInvenItemInfo->Set_ItemDataInit(ItemID);
			m_pInvenItemInfo->Set_Active(true);
		}
	) });

	m_hmapSubHandles.insert({ L"CInventory.EquipItemInfoUnRender",m_pMessageChannel->Subscribe(L"CInvenEquipItem.OnHoverExit",[this](const IMessageChannel::EVENT& Event)
		{
			m_pInvenItemInfo->Set_Active(false);
		}
	) });

	m_hmapSubHandles.insert({ L"Building.Exit", m_pMessageChannel->Subscribe(L"Building.Exit", [this](const IMessageChannel::EVENT& Event)
		{
			auto iter = Event.hmapData.find(L"BuildingType");
			if (iter == Event.hmapData.end())
				return;

			BUILDING_TYPE eType = any_cast<BUILDING_TYPE>(iter->second);

			switch (eType)
			{
			case BT_COOK:
				Use_Item(CItem::IG_WOOD, 2);
				Use_Item(CItem::IG_STONE, 2);
				break;
			case BT_KNUCKLEBONE:
				Use_Item(CItem::IG_GOLD, 2);
				Use_Item(CItem::IG_WOOD, 2);
				break;
			case BT_SHRINE:
				Use_Item(CItem::IG_GOLD, 2);
				Use_Item(CItem::IG_STONE, 2);
				break;
			}
		}
	) });

	m_bMsgRegistered = true;
}

CInventory* CInventory::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CInventory* pInstance = new CInventory(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CInventory Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CInventory::Add_Item(CItem::ITEMID _eid, _int _iCount)
{
	for (auto pItem : m_vItem)
	{
		tItemData ItemData = pItem->Get_ItemData();
		if (ItemData.ID == _eid)
		{
			// UPDATE
			ItemData.iCount += _iCount;
			pItem->Set_ItemData(ItemData);
			pItem->Set_Render(true);
			CPersistentMgr::GetInstance()->Get_ResourceHistory()->AddItem(_eid, _iCount);
			return;
		}
	}

	for (auto pItem : m_vItem)
	{
		tItemData ItemData = pItem->Get_ItemData();
		if (ItemData.ID ==  CItem::ID_END || ItemData.iCount == 0)
		{
			// NEW

			ItemData.ID = _eid;
			ItemData.iCount = _iCount;
			pItem->Set_ItemData(ItemData);
			pItem->Set_LocalPos(m_vSlotLocalPos[m_iCurItemCount]);
			m_iCurItemCount++;
			m_InventoryItemList.push_back(ItemData.ID);
			pItem->Set_Render(true);
			CPersistentMgr::GetInstance()->Get_ResourceHistory()->AddItem(_eid, _iCount);
			return;
		}
	}
}

BOOL CInventory::Use_Item(CItem::ITEMID _eid, _int _iCount)
{
	for (auto pItem : m_vItem)
	{
		tItemData ItemData = pItem->Get_ItemData();
		if (ItemData.ID == _eid)
		{
			if (ItemData.iCount < _iCount)
				return false;
			ItemData.iCount -= _iCount;
			CPersistentMgr::GetInstance()->Get_ResourceHistory()->UseItem(_eid, -_iCount);
			if (ItemData.iCount == 0)
			{
				m_InventoryItemList.remove(ItemData.ID);
				ItemData.ID = CItem::ID_END;
				pItem->Set_Render(false);
				m_iCurItemCount--;
				SortLocalPos();
			}
			pItem->Set_ItemData(ItemData);
			return true;
		}
	}
	return false;
}

_int CInventory::Get_ItemCount(CItem::ITEMID _eid)
{
	for (auto pItem : m_vItem)
	{
		tItemData ItemData = pItem->Get_ItemData();
		if (ItemData.ID == _eid)
		{
			return ItemData.iCount;
		}
	}
	return 0;
}

void CInventory::Free()
{
	Safe_Release(m_pInvenBack);
	Safe_Release(m_pItemInvenBtn);
	Safe_Release(m_pPlayerInfoBtn);
	Safe_Release(m_pInvenItemInfo);
	Safe_Release(m_pInfoRedFog);
	Safe_Release(m_pInfoStarImg);
	Safe_Release(m_pPlayerNameFont);
	Safe_Release(m_pInvenPlyer);
	Safe_Release(m_pInvenPlayerHp);
	Safe_Release(m_pResourceFont);

	for (auto& ptr : m_vSlot)
	{
		Safe_Release(ptr);
	}
	m_vSlot.clear();
	for (auto& ptr : m_vItem)
	{
		Safe_Release(ptr);
	}
	m_vItem.clear();
	for (auto& ptr : m_vPlayerSlot)
	{
		Safe_Release(ptr);
	}
	m_vPlayerSlot.clear();
	for (auto& ptr : m_vPlayerEquipItem)
	{
		Safe_Release(ptr);
	}
	m_vPlayerEquipItem.clear();

	CUi::Free();
}


void CInventory::SortLocalPos()
{
	_int TempCount = 0;
	for (auto InvenItemID : m_InventoryItemList)
	{
		for (auto pItem : m_vItem)
		{
			if (InvenItemID == pItem->Get_ItemData().ID)
			{
				pItem->Set_LocalPos(m_vSlotLocalPos[TempCount]);
				TempCount++;
			}
		}
	}
}
