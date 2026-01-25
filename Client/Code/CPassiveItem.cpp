#include "pch.h"
#include "CPassiveItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CTriggerPoint.h"
#include "CWeaponInfo.h"

CPassiveItem::CPassiveItem(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CItem(pGraphicDev),
		m_bTriggered(false)
{
}

CPassiveItem::CPassiveItem(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	:	CItem(pGraphicDev, StageChannel),
		m_bTriggered(false)
{
}

CPassiveItem::CPassiveItem(const CPassiveItem& rhs)
	:	CItem(rhs),
		m_bTriggered(rhs.m_bTriggered)
{
}

CPassiveItem::~CPassiveItem()
{
}

HRESULT CPassiveItem::Ready_GameObject()
{
	m_eOBJID = OID_ITEM;
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();

	Ready_Event();

	if (m_eItemID == WP_SWORD || m_eItemID == WP_GAUNTLET)
	{
		m_eCurState = IS_IDLE;
		if (m_eItemID == WP_SWORD)
		{
			m_pWeaponInfo = CWeaponInfo::Create(m_pGraphicDev, CWeaponInfo::WINFO_SWORD);
		}
		else if (m_eItemID == WP_GAUNTLET)
		{
			m_pWeaponInfo = CWeaponInfo::Create(m_pGraphicDev, CWeaponInfo::WINFO_GAUNTLET);
		}
	}

	return S_OK;
}

_int CPassiveItem::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		return iExit;
	}

	switch (m_eCurState)
	{
	case IS_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case IS_IDLE:
		Update_Idle(fTimeDelta);
		break;
	case IS_SUMMON:
		Update_Summon(fTimeDelta);
		break;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CPassiveItem::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_pTrigger) {
		m_pTrigger->LateUpdate_GameObject(fTimeDelta);
	}
	if (m_eItemID == WP_SWORD || m_eItemID == WP_GAUNTLET)
	{
		m_pWeaponInfo->LateUpdate_GameObject(fTimeDelta);
	}

	CItem::LateUpdate_GameObject(fTimeDelta);


	if (m_eItemID == WP_SWORD || m_eItemID == WP_GAUNTLET)
	{
		AABB tWeaponAABB = { m_vPos, _vec3(1.f,2.f,1.f) };
		m_pColliderCom->UpdateFromCustom(tWeaponAABB);
	}
	else
	{
		m_pColliderCom->UpdateFromTransform(m_pTransformCom);
	}

	if (g_bDebug) { m_pColliderCom->Update_AABBforRender(); }

	m_pWeaponInfo->UnActive();
}

void CPassiveItem::OnCollision(CGameObject* pObject)
{
	if (m_eCurState == IS_SPAWN)
		return;

	if (pObject->Get_OBJID() == OID_PLAYER)
	{
		m_pWeaponInfo->Active();
		m_bTriggered = true;
	}
}

void CPassiveItem::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Trigger.Activate.Owner", m_pMessageChannel->Subscribe(L"Trigger.Activate.Owner", [this](const IMessageChannel::EVENT& Event)
		{
			auto Owneriter = Event.hmapData.find(L"Trigger_Owner");
			if (Owneriter == Event.hmapData.end()) { return; }

			if (any_cast<CGameObject*>(Owneriter->second) == this)
			{
				m_iHp = 0;
			}
		}
	) });

}

void CPassiveItem::Update_Idle(const _float& fTimeDelta)
{
	if (m_pTrigger == nullptr)
	{
		switch (m_eItemID)
		{
		case FD_GFOOD:
			m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, m_vPos, _vec3(1.f, 1.f, 1.f), Trigger::TI_ITEM, L"GoodFood", false, this);
			break;
		case FD_BFOOD:
			m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, m_vPos, _vec3(1.f, 1.f, 1.f), Trigger::TI_ITEM, L"BadFood", false, this);
			break;
		case WP_SWORD:
			m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, m_vPos, _vec3(1.f, 2.f, 1.f), Trigger::TI_ITEM, L"Sword", false, this);
			break;
		case WP_GAUNTLET:
			m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, m_vPos, _vec3(1.f, 2.f, 1.f), Trigger::TI_ITEM, L"Gauntlet", false, this);
			break;
		}
	}
	m_fAcmlTime += fTimeDelta;
	m_pTrigger->Set_Pos_Trigger(m_vPos);
	m_pTrigger->Update_GameObject(fTimeDelta);
	if (m_eItemID == WP_SWORD || m_eItemID == WP_GAUNTLET)
	{
		m_pWeaponInfo->Update_GameObject(fTimeDelta);
	}
}

void CPassiveItem::Update_Summon(const _float& fTimeDelta)
{
	m_fAcmlTime += fTimeDelta;

	if (m_fAcmlTime >= 3.f)
	{
		m_eCurState = IS_IDLE;
	}
}

void CPassiveItem::Interact()
{
	if (!m_bTriggered) return;

	if (FD_GFOOD <= m_eItemID || m_eItemID <= FD_BFOOD)
	{
		// 플레이어에게 버프 부여
		m_iHp = 0;
		return;
	}
	
	if (WP_SWORD <= m_eItemID || m_eItemID <= WP_TENTACLE)
	{
		// 플레이어의 무기 슬롯에 장착
		return;
	}

}

CPassiveItem* CPassiveItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, ITEMID eID, _float fThrowRange)
{
	CPassiveItem* pItem = new CPassiveItem(pGraphicDev, StageChannel);

	pItem->m_fThrowRange = fThrowRange;
	pItem->m_eItemID = eID;

	if (FAILED(pItem->Ready_GameObject()))
	{
		MSG_BOX("CPassiveItem Create Failed");
		Safe_Release(pItem);
		return nullptr;
	}

	return pItem;
}

void CPassiveItem::Free()
{
	Safe_Destroy(m_pTrigger);
	Safe_Release(m_pWeaponInfo);
	CItem::Free();
}
