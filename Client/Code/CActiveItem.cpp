#include "pch.h"
#include "CActiveItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CSoundMgr.h"

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
	if(m_eCurState == IS_SPAWN)
		return;

	if(pObject->Get_OBJID() == OID_PLAYER)
	{
		// 아이템 획득 처리
		m_iHp = 0;
		_tchar strSoundName[128] = L"";
		swprintf_s(strSoundName, L"ItemPickup%d.wav", Get_Rand_Int(1, 7));
		CSoundMgr::GetInstance()->Play(strSoundName, SOUND_EFFECT, 0.5f);
	}
}

void CActiveItem::Update_Idle(const _float& fTimeDelta)
{
	m_fAcmlTime += fTimeDelta;

	if (m_fAcmlTime >= 1.f)
	{
		m_eCurState = IS_CHASE;
	}
}

void CActiveItem::Update_Chase(const _float& fTimeDelta)
{
	CTransform* pTransformCom = CPersistentMgr::GetInstance()->Get_PlayerTransform();

	NULL_CHECK(pTransformCom);

	_vec3 vTargetPos;

	pTransformCom->Get_Info(INFO_POS, &vTargetPos);

	m_vSpeed = vTargetPos - m_vPos;

	m_pTransformCom->Chase_Target(&vTargetPos, fTimeDelta, 7.f * D3DXVec3Length(&m_vSpeed));
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
