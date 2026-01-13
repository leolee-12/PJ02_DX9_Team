#include "pch.h"
#include "CN1_AI.h"
#include "CTransform.h"
#include "CItem.h"
#include "CB2_AI.h"

CN1_AI::CN1_AI(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CAIController(pGraphicDev),
		m_fSpeed(0.f),
		m_fAcmlTime(0.f),
		m_bChase(false)
{
}

CN1_AI::CN1_AI(const CN1_AI& rhs)
	:	CAIController(rhs),
		m_fSpeed(rhs.m_fSpeed),
		m_fAcmlTime(rhs.m_fAcmlTime),
		m_bChase(false)
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
		break;
	case CMonsterN1::N1S_RUN:
	{
		if (m_pTargetTC) m_fSpeed = 2.f;
		else m_fSpeed = 0.5f;

		if ((!m_bChase) || (m_fAcmlTime < 2.f))
		{
			m_vDir = Randomize_Dir();
		}
		else if (m_bChase)
		{
			if (m_fAcmlTime >= 2.f || m_iPreState == CMonsterN1::N1S_HIT)
				m_vDir = Compute_TargetDir();
		}
	}
		break;
	case CMonsterN1::N1S_ATTACK:
	{
		m_fSpeed = 0.f;
		m_pOwnerTC->Get_Info(INFO_POS, &m_vLerpPos);
		m_vLerpPos += m_vDir * 3.f;
	}
		break;
	case CMonsterN1::N1S_HIT:
	{
		m_fSpeed = 0.1f;
		Compute_TargetDir();
		m_pOwnerTC->Get_Info(INFO_POS, &m_vLerpPos);
		m_vLerpPos -= m_vDir * 2.f;
	}
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
		if (!m_pTargetTC) m_bChase = false;
		break;
	case CMonsterN1::N1S_RUN:
		if (!m_pTargetTC) m_bChase = false;
		break;
	case CMonsterN1::N1S_ATTACK:
		if (!m_pTargetTC) m_bChase = false;
		m_fAcmlTime = 0.f;
		break;
	case CMonsterN1::N1S_HIT:
		if (!m_pTargetTC) m_bChase = false;
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
	if (!m_pTargetTC) return;

	if (m_bChase)
	{	// 타겟을 이미 발견했을 때
		if ((m_fDistance <= m_fInteractRange) && (m_fAcmlTime >= 5.f))
		{	// 타겟이 상호작용 범위 내에 있을 시 공격 상태로 전환
			Change_State(CMonsterN1::N1S_ATTACK);
			return;
		}
		else
		{	
			if (_uint(m_fAcmlTime) % 3 != 0)	// 타겟이 상호작용 범위 내에 없을 시 이동 상태로 전환하여 추적
				Change_State(CMonsterN1::N1S_RUN);
		}
	}
	else
	{	
		if (m_fDistance <= m_fDetectRange)
		{	// 타겟이 감지 범위 내로 진입 시 발견 후 이동 상태로 전환하여 추적
			m_bChase = true;
			Change_State(CMonsterN1::N1S_RUN);
		}
		else if (_uint(m_fAcmlTime) % 3 != 0)
		{	// 타겟이 감지 범위 내에 없을 때는 대기하다 이동 상태로 전환하여 순찰
			Change_State(CMonsterN1::N1S_RUN);
		}
	}
}

void CN1_AI::Update_Run(const _float& fTimeDelta)
{
	if (!m_pTargetTC) Change_State(CMonsterN1::N1S_IDLE);

	if (m_bChase)
	{	// 타겟을 이미 발견했을 때
		if ((m_fDistance <= m_fInteractRange) && (m_fAcmlTime >= 5.f))
		{	
			Change_State(CMonsterN1::N1S_ATTACK);
		}
	}
	else
	{	// 타겟을 발견하지 못했을 때
		if (m_fDistance <= m_fDetectRange)
		{	// 타겟이 감지 범위 내로 진입 시 발견
			m_bChase = true;
		}
	}

	if (_uint(m_fAcmlTime) % 3 == 0)
	{	// 타겟이 감지 범위 내에 없을 때는 순찰하다 대기 상태로 전환
		Change_State(CMonsterN1::N1S_IDLE);
		return;
	}

	m_pOwnerTC->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
}

void CN1_AI::Update_Attack(const _float& fTimeDelta)
{
	if (!m_pTargetTC) Change_State(CMonsterN1::N1S_IDLE);

	_vec3 vPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vPos);
	D3DXVec3Lerp(&vPos, &vPos, &m_vLerpPos, m_fSpeed);
	m_pOwnerTC->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CN1_AI::Update_Hit(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vPos);
	D3DXVec3Lerp(&vPos, &vPos, &m_vLerpPos, m_fSpeed);
	m_pOwnerTC->Set_Pos(vPos.x, vPos.y, vPos.z);
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

void CN1_AI::Anim_End(CMonsterN1::MONSTER_N1_STATE eState)
{
	switch (eState)
	{
	case CMonsterN1::N1S_ATTACK:
		Change_State(CMonsterN1::N1S_RUN);
		break;

	case CMonsterN1::N1S_HIT:
		Change_State(CMonsterN1::N1S_RUN);
		break;

	case CMonsterN1::N1S_SPAWN:
		Change_State(CMonsterN1::N1S_IDLE);
		break;
	}
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