#include "pch.h"
#include "CN1_AI.h"
#include "CMonsterN1.h"

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

HRESULT CN1_AI::Ready_AI(CGameObject* pOwner, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	if (FAILED(CAIController::Ready_AI(pOwner, fDetectRange, fInteractRange, iInitState)))
		return E_FAIL;

	m_fSpeed = 0.5f;
	m_fAcmlTime = 0.f;
	m_iRcmState = _uint(CMonsterN1::N1S_IDLE);

	return S_OK;
}

void CN1_AI::Change_State(const _uint& iState)
{
	if(m_iCurState == iState) return;

	// Exit Current State
	
	m_iPreState = m_iCurState;

	// Enter New State
}

void CN1_AI::Enter_State(const _uint& iState)
{
}

void CN1_AI::Exit_State(const _uint& iState)
{
}

_int CN1_AI::Update_Component(const _float& fTimeDelta)
{
	_int iExit(0);

	if (!m_bActiveAI) return iExit;

	Compute_Distance();

	switch (m_iCurState)
	{
	case CMonsterN1::N1S_IDLE:
		iExit = Update_Idle(fTimeDelta);
		break;
	case CMonsterN1::N1S_RUN:
		iExit = Update_Run(fTimeDelta);
		break;
	case CMonsterN1::N1S_ATTACK:
		iExit = Update_Attack(fTimeDelta);
		break;
	case CMonsterN1::N1S_HIT:
		iExit = Update_Hit(fTimeDelta);
		break;
	case CMonsterN1::N1S_SPAWN:
		iExit = Update_Spawn(fTimeDelta);
		break;
	case CMonsterN1::N1S_JEER:
		iExit = Update_Jeer(fTimeDelta);
		break;
	case CMonsterN1::N1S_PRAY:
		iExit = Update_Pray(fTimeDelta);
		break;
	}

	return iExit;
}

_int CN1_AI::Update_Idle(const _float& fTimeDelta)
{
	return _int();
}

_int CN1_AI::Update_Run(const _float& fTimeDelta)
{
	return _int();
}

_int CN1_AI::Update_Attack(const _float& fTimeDelta)
{
	return _int();
}

_int CN1_AI::Update_Hit(const _float& fTimeDelta)
{
	return _int();
}

_int CN1_AI::Update_Spawn(const _float& fTimeDelta)
{
	return _int();
}

_int CN1_AI::Update_Jeer(const _float& fTimeDelta)
{
	return _int();
}

_int CN1_AI::Update_Pray(const _float& fTimeDelta)
{
	return _int();
}

_float CN1_AI::Compute_Distance()
{
	return _float();
}

CN1_AI* CN1_AI::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* pOwner, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	CN1_AI* pN1_AI = new CN1_AI(pGraphicDev);

	if (FAILED(pN1_AI->Ready_AI(pOwner, fDetectRange, fInteractRange, iInitState)))
	{
		Safe_Release(pN1_AI);
		MSG_BOX("CN1_AI Create Failed");
		return nullptr;
	}

	return pN1_AI;
}

CComponent* CN1_AI::Clone()
{
	return nullptr;
}

void CN1_AI::Free()
{
}