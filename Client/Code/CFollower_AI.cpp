#include "pch.h"
#include "CFollower_AI.h"
#include "CTransform.h"
#include "CItem.h"

CFollower_AI::CFollower_AI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CAIController(pGraphicDev),
	m_fSpeed(0.f),
	m_fAcmlTime(0.f),
	m_bChase(false)
{
}

CFollower_AI::CFollower_AI(const CFollower_AI& rhs)
	: CAIController(rhs),
	m_fSpeed(rhs.m_fSpeed),
	m_fAcmlTime(rhs.m_fAcmlTime),
	m_bChase(false)
{
}

CFollower_AI::~CFollower_AI()
{
}

HRESULT CFollower_AI::Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	if (FAILED(CAIController::Ready_AI(fDetectRange, fInteractRange, iInitState)))
		return E_FAIL;

	m_fSpeed = FW_DEFAULT_SPEED;
	m_fAcmlTime = 0.f;
	m_iRcmState = _uint(CFollower::FOLLOWER_IDLE);

	return S_OK;
}

void CFollower_AI::Enter_State(const _uint& iState)
{
	switch (iState)
	{
	case CFollower::FOLLOWER_IDLE:
		m_fAcmlTime = 0.f;
		break;
	case CFollower::FOLLOWER_RUN:
	{
		m_fAcmlTime = 0.f;

		if (!m_bChase || m_pTargetTC == nullptr) m_vDir = Randomize_Dir();
		else									 m_vDir = Compute_TargetDir();
	}
		break;

	case CFollower::FOLLOWER_DANCE:
		m_fAcmlTime = 0.f;
		break;

	case CFollower::FOLLOWER_CHEER:
		m_fAcmlTime = 0.f;
		break;

	case CFollower::FOLLOWER_TRANSFORM:
		break;
	case CFollower::FOLLOWER_UNCONVERT:
		break;
	case CFollower::FOLLOWER_CONVERT:
		break;
	case CFollower::FOLLOWER_ACTION:
		break;
	case CFollower::FOLLOWER_RECRUIT:
		break;
	}
}

void CFollower_AI::Exit_State(const _uint& iState)
{
	switch (iState)
	{
	case CFollower::FOLLOWER_IDLE:
		break;
	case CFollower::FOLLOWER_RUN:
		break;
	case CFollower::FOLLOWER_DANCE:
		break;
	case CFollower::FOLLOWER_TRANSFORM:
		break;
	case CFollower::FOLLOWER_UNCONVERT:
		break;
	case CFollower::FOLLOWER_CONVERT:
		break;
	case CFollower::FOLLOWER_ACTION:
		m_pTargetTC = nullptr;
		break;
	case CFollower::FOLLOWER_RECRUIT:
		m_bActiveAI = true;
		break;
	}
}

_int CFollower_AI::Update_Component(const _float& fTimeDelta)
{
	if (m_pTargetTC)	m_bChase = true;
	else				m_bChase = false;

	_int iExit(0);

	m_fAcmlTime += fTimeDelta;

	if (!m_bActiveAI) return iExit;

	Compute_Distance();

	switch (m_iCurState)
	{
	case CFollower::FOLLOWER_IDLE:
		Update_Idle(fTimeDelta);
		break;
	case CFollower::FOLLOWER_RUN:
		Update_Run(fTimeDelta);
		break;
	case CFollower::FOLLOWER_DANCE:
		Update_Dance(fTimeDelta);
		break;
	case CFollower::FOLLOWER_TRANSFORM:
		Update_Transform(fTimeDelta);
		break;
	case CFollower::FOLLOWER_UNCONVERT:
		Update_Unconvert(fTimeDelta);
		break;
	case CFollower::FOLLOWER_CONVERT:
		Update_Convert(fTimeDelta);
		break;
	case CFollower::FOLLOWER_ACTION:
		Update_Action(fTimeDelta);
		break;
	case CFollower::FOLLOWER_RECRUIT:
		Update_Recruit(fTimeDelta);
		break;
	}

	return iExit;
}

void CFollower_AI::Update_Idle(const _float& fTimeDelta)
{
	if ((!m_bChase) || (m_pTargetTC == nullptr))
	{
		if (m_fAcmlTime >= AUTO_ESCAPE_STATE_TIME)
		{
			_uint iRand = Get_Rand_Int(0, 4);

			if (iRand == 0) Change_State(CFollower::FOLLOWER_DANCE);
			else			Change_State(CFollower::FOLLOWER_RUN);
		}
	}
	else Change_State(CFollower::FOLLOWER_RUN);
}

void CFollower_AI::Update_Run(const _float& fTimeDelta)
{
	if ((!m_bChase) || (m_pTargetTC == nullptr))
	{
		if (m_fAcmlTime >= AUTO_ESCAPE_STATE_TIME)
		{
			Change_State(CFollower::FOLLOWER_IDLE);
			return;
		}
	}
	else if (m_fDistance <= m_fInteractRange)
	{
		_vec3 vOwnerPos, vTargetPos;
		m_pOwnerTC->Get_Info(INFO_POS, &vOwnerPos);
		m_pTargetTC->Get_Info(INFO_POS, &vTargetPos);

		if (vOwnerPos.z < vTargetPos.z)
		{
			Change_State(CFollower::FOLLOWER_ACTION);
			return;
		}
	}

	m_pOwnerTC->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
}

void CFollower_AI::Update_Dance(const _float& fTimeDelta)
{
	if ((!m_bChase) || (m_pTargetTC == nullptr))
	{
		if (m_fAcmlTime >= AUTO_ESCAPE_STATE_TIME)
		{
			Change_State(CFollower::FOLLOWER_IDLE);
			return;
		}
	}
}

void CFollower_AI::Update_Cheer(const _float& fTimeDelta)
{
}

void CFollower_AI::Update_Transform(const _float& fTimeDelta)
{
}

void CFollower_AI::Update_Unconvert(const _float& fTimeDelta)
{
}

void CFollower_AI::Update_Convert(const _float& fTimeDelta)
{
}

void CFollower_AI::Update_Action(const _float& fTimeDelta)
{
}

void CFollower_AI::Update_Recruit(const _float& fTimeDelta)
{
}



void CFollower_AI::Anim_End(CFollower::FOLLOWER_STATE eState)
{
	switch (eState)
	{
	case CFollower::FOLLOWER_IDLE:
		break;
	case CFollower::FOLLOWER_RUN:
		break;
	case CFollower::FOLLOWER_DANCE:
		break;
	case CFollower::FOLLOWER_CHEER:
		break;
	case CFollower::FOLLOWER_TRANSFORM:
		break;
	case CFollower::FOLLOWER_UNCONVERT:
		break;
	case CFollower::FOLLOWER_CONVERT:
		break;
	case CFollower::FOLLOWER_ACTION:
		Change_State(CFollower::FOLLOWER_IDLE);
		break;
	case CFollower::FOLLOWER_RECRUIT:
		Change_State(CFollower::FOLLOWER_IDLE);
		break;
	}
}

CFollower_AI* CFollower_AI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	CFollower_AI* pFollower_AI = new CFollower_AI(pGraphicDev);

	if (FAILED(pFollower_AI->Ready_AI(fDetectRange, fInteractRange, iInitState)))
	{
		Safe_Release(pFollower_AI);
		MSG_BOX("pFollower_AI Create Failed");
		return nullptr;
	}

	return pFollower_AI;
}

CComponent* CFollower_AI::Clone()
{
	return new CFollower_AI(*this);
}

void CFollower_AI::Free()
{
}
