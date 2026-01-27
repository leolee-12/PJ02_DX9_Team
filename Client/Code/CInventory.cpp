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


CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
}

CInventory::~CInventory()
{

}

HRESULT CInventory::Ready_GameObject()
{
	m_vPos = _vec3{ 0,0,0 };
	m_pInvenBack = CInvenBack::Create(m_pGraphicDev,_vec3(-WINCX / 4,0.0f,0.1f), m_vPos,1.0f);
	if (nullptr == m_pInvenBack)	
		return E_FAIL;

	m_pItemInvenBtn = CInvenBtn::Create(m_pGraphicDev, _vec3(-480.0f, 190.0f, 0.01f), m_vPos, 0.4f);
	if (nullptr == m_pItemInvenBtn)
		return E_FAIL;
	m_pItemInvenBtn->Set_Tex(L"소모품");

	m_pPlayerInfoBtn = CInvenBtn::Create(m_pGraphicDev, _vec3(-360.0f, 190.0f, 0.01f), m_vPos, 0.4f);
	if (nullptr == m_pPlayerInfoBtn)
		return E_FAIL;
	m_pPlayerInfoBtn->Set_Tex(L"정보창");

	CInvenSlot* pSlot;
	CInvenItem* pItem;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{

			pSlot = CInvenSlot::Create(m_pGraphicDev, { -520.f + (75.f * j), 80.f - (75.f * i), 0.01f }, m_vPos, 0.3f);
			pItem = CInvenItem::Create(m_pGraphicDev, { -520.f + (75.f * j), 80.f - (75.f * i), 0.01f }, m_vPos, 0.3f);
			if (nullptr == pSlot)
				return E_FAIL;

			m_vSlotLocalPos.push_back(_vec3{ -520.f + (75.f * j), 80.f - (75.f * i), 0.01f });
			m_vSlot.push_back(pSlot);
			m_vItem.push_back(pItem);

		}
	}

	m_eCurInvenState = IS_END;
	m_ePreInvenState = IS_END;
	m_fLerpTime = 1.0f;
	m_vLerpStart = { 0,0,0 };
	m_vLerpEnd = { 0,0,0 };
	m_iInvenCount = 0;
	Add_Item(CItem::IG_BERRY, 20);
	return S_OK;
}

_int CInventory::Update_GameObject(const _float& fTimeDelta)
{
	Key_Input_Inven();
	if (!m_bActive) { return NOEVENT; }


	if (m_eCurInvenState != m_ePreInvenState)
	{
		m_ePreInvenState = m_eCurInvenState;
		switch (m_eCurInvenState)
		{
		case IS_OPEN:
			Moveto(m_vPos, _vec3{ -700,0,0 });
			break;
		case IS_STAY:
			break;
		case IS_CLOSE:
			Moveto(m_vPos, _vec3{ 0,0,0 });
			break;
		case IS_END:
			break;
		default:
			break;
		}
	}

	if (m_eCurInvenState != IS_STAY)
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
			Set_State(IS_STAY);
		}
	}

	m_pPlayerInfoBtn->Set_ParentPos(m_vPos);
	m_pItemInvenBtn->Set_ParentPos(m_vPos);
	m_pInvenBack->Set_ParentPos(m_vPos);
	m_pPlayerInfoBtn->Update_GameObject(fTimeDelta);
	m_pItemInvenBtn->Update_GameObject(fTimeDelta);
	m_pInvenBack->Update_GameObject(fTimeDelta);

	for (auto it : m_vSlot)
	{
		it->Update_GameObject(fTimeDelta);
		it->Set_ParentPos(m_vPos);
	}
	for (auto it : m_vItem)
	{
		it->Update_GameObject(fTimeDelta);
		it->Set_ParentPos(m_vPos);
	}
	return NOEVENT;
}

void CInventory::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return; }
	m_pPlayerInfoBtn->LateUpdate_GameObject(fTimeDelta);
	m_pItemInvenBtn->LateUpdate_GameObject(fTimeDelta);
	m_pInvenBack->LateUpdate_GameObject(fTimeDelta);
	for (auto it : m_vSlot)
	{
		it->LateUpdate_GameObject(fTimeDelta);
	}
	for (auto it : m_vItem)
	{
		it->LateUpdate_GameObject(fTimeDelta);
	}
}

void CInventory::Render_GameObject()
{
	if (!m_bActive) { return; }
	m_pPlayerInfoBtn->Render_GameObject();
	m_pItemInvenBtn->Render_GameObject();
	m_pInvenBack->Render_GameObject();
	for (auto it : m_vSlot)
	{
		it->Render_GameObject();
	}
	for (auto it : m_vItem)
	{
		it->Render_GameObject();
	}
}

void CInventory::Key_Input_Inven()
{
	// 디버기용 인풋
	if (CDInputMgr::GetInstance()->Key_Down(DIK_L))
	{
		m_bActive = !m_bActive;
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_Q))
	{
		Set_State(IS_OPEN);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_R))
	{
		Set_State(IS_CLOSE);
	}
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

void CInventory::Ready_Event()
{
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
		if (pItem->Get_ItemID() == _eid)
		{
			// UPDATE
			pItem->Set_ItemCount(pItem->Get_ItemCount() + _iCount);
			pItem->Set_Render(true);
			return;
		}
	}
	for (auto pItem : m_vItem)
	{

		if (pItem->Get_ItemID() ==  CItem::ID_END || pItem->Get_ItemCount() == 0)
		{
			// NEW
			pItem->Set_ItemID(_eid);
			pItem->Set_ItemCount(_iCount);
			pItem->Set_LocalPos(m_vSlotLocalPos[m_iInvenCount]);
			m_iInvenCount++;
			pItem->Set_Render(true);
			return;
		}
	}
}

BOOL CInventory::Use_Item(CItem::ITEMID _eid, _int _iCount)
{
	for (auto pItem : m_vItem)
	{
		if (pItem->Get_ItemID() == _eid)
		{
			if (pItem->Get_ItemCount()- _iCount  < 0)
				return false;

			pItem->Set_ItemCount(pItem->Get_ItemCount() - _iCount);
			CPersistentMgr::GetInstance()->Get_ResourceHistory()->UseItem(TYPE_BERRY, -_iCount);
			if (pItem->Get_ItemCount() == 0)
			{
				pItem->Set_ItemID(CItem::ID_END);
				pItem->Set_Render(false);
				m_iInvenCount--;
				SortLocalPos();
			}
			return true;
		}
	}
	return false;
}

_int CInventory::GetItemCount(CItem::ITEMID _eid)
{
	for (auto pItem : m_vItem)
	{
		if (pItem->Get_ItemID() == _eid)
		{
			return pItem->Get_ItemCount();
		}
	}
	return 0;
}

void CInventory::Free()
{
	Safe_Release(m_pInvenBack);
	Safe_Release(m_pItemInvenBtn);
	Safe_Release(m_pPlayerInfoBtn);

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

	CUi::Free();
}

void CInventory::SortLocalPos()
{
	_int TempCount = 0;
	for (auto pItem : m_vItem)
	{
		if (pItem->Get_ItemID() != CItem::ID_END && 0 < pItem->Get_ItemCount())
		{
			pItem->Set_LocalPos(m_vSlotLocalPos[TempCount]);
			TempCount++;
		}
	}
}

