#include "pch.h"
#include "CActiveItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CSoundMgr.h"
#include "CEffectMgr.h"

CActiveItem::CActiveItem(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CItem(pGraphicDev)
{
}

CActiveItem::CActiveItem(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	:	CItem(pGraphicDev, StageChannel)
{
}

CActiveItem::CActiveItem(const CActiveItem& rhs)
	:	CItem(rhs)
{
}

CActiveItem::~CActiveItem()
{
}

HRESULT CActiveItem::Ready_GameObject()
{
	if (FAILED(CItem::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

_int CActiveItem::Update_GameObject(const _float& fTimeDelta)
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
	case IS_CHASE:
		Update_Chase(fTimeDelta);
		break;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CActiveItem::OnCollision(CGameObject* pObject)
{
	if (m_iHp <= 0) { return; }
	if(m_eCurState == IS_SPAWN)
		return;

	if(pObject->Get_OBJID() == OID_PLAYER)
	{
		// 아이템 획득 처리
		IMessageChannel::EVENT tEvent;

		switch (m_eItemID)
		{
		case IG_PASSION:
			tEvent.strType = L"Player.AddPassion";
			m_pMessageChannel->Publish(tEvent);
			break;

		default:
			tEvent.strType = L"ResourceHistory.AddItem";
			tEvent.hmapData[L"ItemID"] = (int)m_eItemID;
			m_pMessageChannel->Publish(tEvent);
			break;
		}

		m_iHp = 0;
		_tchar strSoundName[128] = L"";
		swprintf_s(strSoundName, L"ItemPickup%d.wav", Get_Rand_Int(1, 7));
		CSoundMgr::GetInstance()->Play(strSoundName, SOUND_EFFECT, 0.5f);
		CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_PICKUP, 0, _vec3(m_vPos.x, m_vPos.y + 1.f, m_vPos.z - 0.5f), _vec3(0.5f, 0.2f, 0.f));
		CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_PICKUP, 1, _vec3(m_vPos.x, m_vPos.y + 1.f, m_vPos.z - 0.5f), _vec3(0.5f, 0.2f, 0.f));
	}
}

void CActiveItem::Update_Idle(const _float& fTimeDelta)
{
	m_fAcmlTime += fTimeDelta;

	if (m_fAcmlTime >= 1.f)
	{
		CTransform* pTransformCom = CPersistentMgr::GetInstance()->Get_PlayerTransform();

		NULL_CHECK(pTransformCom);

		_vec3 vTargetPos;

		pTransformCom->Get_Info(INFO_POS, &vTargetPos);

		vTargetPos -= m_vPos;

		_float fDist = D3DXVec3Length(&vTargetPos);

		if (fDist <= AUTO_CHASE_RANGE)	m_eCurState = IS_CHASE;
		else							m_fAcmlTime = 0.f;
	}
}

void CActiveItem::Update_Chase(const _float& fTimeDelta)
{
	CTransform* pTransformCom = CPersistentMgr::GetInstance()->Get_PlayerTransform();

	NULL_CHECK(pTransformCom);

	_vec3 vTargetPos;

	pTransformCom->Get_Info(INFO_POS, &vTargetPos);

	m_vSpeed = vTargetPos - m_vPos;

	m_pTransformCom->Chase_Target(&vTargetPos, fTimeDelta, 7.5f * D3DXVec3Length(&m_vSpeed));
}

CActiveItem* CActiveItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _float fThrowRange)
{
	CActiveItem* pItem = new CActiveItem(pGraphicDev, StageChannel);

	pItem->m_fThrowRange = fThrowRange;

	if (FAILED(pItem->Ready_GameObject()))
	{
		MSG_BOX("CActiveItem Create Failed");
		Safe_Release(pItem);
		return nullptr;
	}

	return pItem;
}

void CActiveItem::Free()
{
	CItem::Free();
}
