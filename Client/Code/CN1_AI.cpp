#include "pch.h"
#include "CN1_AI.h"
#include "CMonsterN1.h"
#include "CTransform.h"

CN1_AI::CN1_AI(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CAIController(pGraphicDev),
		m_fSpeed(0.f),
		m_fAcmlTime(0.f)
{
}

CN1_AI::CN1_AI(const CN1_AI& rhs)
	:	CAIController(rhs),
		m_fSpeed(rhs.m_fSpeed),
		m_fAcmlTime(rhs.m_fAcmlTime)
{
}

CN1_AI::~CN1_AI()
{
}

HRESULT CN1_AI::Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	if (FAILED(CAIController::Ready_AI(fDetectRange, fInteractRange, iInitState)))
		return E_FAIL;

	m_fSpeed = 0.5f;
	m_fAcmlTime = 0.f;
	m_iRcmState = _uint(CMonsterN1::N1S_IDLE);

	return S_OK;
}

void CN1_AI::Enter_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterN1::N1S_IDLE:
		m_fAcmlTime = 0.f;
		break;
	case CMonsterN1::N1S_RUN:
	{
		m_fAcmlTime = 0.f;

		if (m_pTargetTC) m_fSpeed = 2.f;
		else m_fSpeed = 0.5f;
	}
		break;
	case CMonsterN1::N1S_ATTACK:
		m_fAcmlTime = 0.f;
		break;
	case CMonsterN1::N1S_HIT:
		break;
	case CMonsterN1::N1S_SPAWN:
		m_bActiveAI = false;
		break;
	case CMonsterN1::N1S_JEER:
		break;
	case CMonsterN1::N1S_PRAY:
		break;
	}
}

void CN1_AI::Exit_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterN1::N1S_IDLE:
		break;
	case CMonsterN1::N1S_RUN:
		break;
	case CMonsterN1::N1S_ATTACK:
		break;
	case CMonsterN1::N1S_HIT:
		break;
	case CMonsterN1::N1S_SPAWN:
		m_bActiveAI = true;
		break;
	case CMonsterN1::N1S_JEER:
		break;
	case CMonsterN1::N1S_PRAY:
		break;
	}
}

_int CN1_AI::Update_Component(const _float& fTimeDelta)
{
	_int iExit(0);

	m_fAcmlTime += fTimeDelta;

	if (!m_bActiveAI) return iExit;

	Compute_Distance();

	switch (m_iCurState)
	{
	case CMonsterN1::N1S_IDLE:
		Update_Idle(fTimeDelta);
		break;
	case CMonsterN1::N1S_RUN:
		Update_Run(fTimeDelta);
		break;
	case CMonsterN1::N1S_ATTACK:
		Update_Attack(fTimeDelta);
		break;
	case CMonsterN1::N1S_HIT:
		Update_Hit(fTimeDelta);
		break;
	case CMonsterN1::N1S_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case CMonsterN1::N1S_JEER:
		Update_Jeer(fTimeDelta);
		break;
	case CMonsterN1::N1S_PRAY:
		Update_Pray(fTimeDelta);
		break;
	}

	return iExit;
}

void CN1_AI::Update_Idle(const _float& fTimeDelta)
{
	if ((m_pTargetTC != nullptr && m_fDistance <= m_fDetectRange) || m_fAcmlTime > 1.f)
	{	// 타겟이 존재하며, 감지 범위 내에 있을 시
		// IDLE 상태로 일정 시간(1초) 경과 시
		Change_State(CMonsterN1::N1S_RUN);
	}

	// IDLE 상태 : 특정 행동 없음
}

void CN1_AI::Update_Run(const _float& fTimeDelta)
{
	if (((m_pTargetTC == nullptr) && (m_fAcmlTime > 1.f)) || (m_fDistance > m_fDetectRange))
	{	// 타겟이 없으며, Run 상태로 일정 시간(1초) 경과 시
		// 타겟이 감지 범위를 벗어났을 시
		Change_State(CMonsterN1::N1S_IDLE);
	}

	if ((m_fDistance <= m_fInteractRange) && (m_fAcmlTime > 1.f))
	{	// 타겟이 상호작용 범위 내에 있으며, Run 상태로 일정 시간(1초) 경과 시
		Change_State(CMonsterN1::N1S_ATTACK);
	}

	// RUN 상태 : 타겟을 향해 이동
	if (m_pTargetTC)
	{
		m_pOwnerTC->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
	}
}

void CN1_AI::Update_Attack(const _float& fTimeDelta)
{
	if (!m_pTargetTC)
	{
		Change_State(CMonsterN1::N1S_IDLE);
	}
}

void CN1_AI::Update_Hit(const _float& fTimeDelta)
{
}

void CN1_AI::Update_Spawn(const _float& fTimeDelta)
{
}

void CN1_AI::Update_Jeer(const _float& fTimeDelta)
{
}

void CN1_AI::Update_Pray(const _float& fTimeDelta)
{
}

_float CN1_AI::Compute_Distance()
{
	if (!m_pOwnerTC || !m_pTargetTC)
	{
		m_fDistance = FLT_MAX;
		return m_fDistance;
	}

	_vec3 vOwnerPos, vTargetPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vOwnerPos);
	m_pTargetTC->Get_Info(INFO_POS, &vTargetPos);

	m_vDir = vTargetPos - vOwnerPos;
	m_fDistance = D3DXVec3Length(&m_vDir);
	D3DXVec3Normalize(&m_vDir, &m_vDir);
	return _float();
}

CN1_AI* CN1_AI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	CN1_AI* pN1_AI = new CN1_AI(pGraphicDev);

	if (FAILED(pN1_AI->Ready_AI(fDetectRange, fInteractRange, iInitState)))
	{
		Safe_Release(pN1_AI);
		MSG_BOX("CN1_AI Create Failed");
		return nullptr;
	}

	return pN1_AI;
}

CComponent* CN1_AI::Clone()
{
	return new CN1_AI(*this);
}

void CN1_AI::Free()
{
}