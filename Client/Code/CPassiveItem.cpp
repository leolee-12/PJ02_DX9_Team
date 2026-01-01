#include "pch.h"
#include "CPassiveItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"

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
	if (FAILED(CItem::Ready_GameObject()))
		return E_FAIL;

	Ready_Variable();

	Ready_Event();

	return S_OK;
}

_int CPassiveItem::Update_GameObject(const _float& fTimeDelta)
{
	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

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

void CPassiveItem::OnCollision(CGameObject* pObject)
{
	if (m_eCurState == IS_SPAWN)
		return;

	if (pObject->Get_OBJID() == OID_PLAYER)
	{
		// 상호작용 가능
		m_bTriggered = true;
	}
}

HRESULT CPassiveItem::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ItemTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CPassiveItem::Update_Idle(const _float& fTimeDelta)
{
	m_fAcmlTime += fTimeDelta;
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

CPassiveItem* CPassiveItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
	CPassiveItem* pItem = new CPassiveItem(pGraphicDev, StageChannel);

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
	CItem::Free();
}