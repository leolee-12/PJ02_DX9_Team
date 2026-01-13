#include "pch.h"
#include "CB2_AI.h"
#include "CTransform.h"

CB2_AI::CB2_AI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CAIController(pGraphicDev),
	m_fSpeed(0.f),
	m_fAcmlTime(0.f),
	m_bChase(false),
	m_fAngle(0.f),
	m_fGravity(0.f),
	m_iDequeMinSize(3),
	m_pOwner(nullptr),
	m_bOnce(false)
{
	m_vecAtkPatterns.reserve(4);
}

CB2_AI::CB2_AI(const CB2_AI& rhs)
	: CAIController(rhs),
	m_fSpeed(rhs.m_fSpeed),
	m_fAcmlTime(rhs.m_fAcmlTime),
	m_bChase(false),
	m_fAngle(rhs.m_fAngle),
	m_fGravity(rhs.m_fGravity),
	m_iDequeMinSize(rhs.m_iDequeMinSize),
	m_pOwner(nullptr),
	m_bOnce(false)
{
	m_vecAtkPatterns = rhs.m_vecAtkPatterns;
	m_patternDeque = rhs.m_patternDeque;
}

CB2_AI::~CB2_AI()
{
}

HRESULT CB2_AI::Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	if (FAILED(CAIController::Ready_AI(fDetectRange, fInteractRange, iInitState)))
		return E_FAIL;

	m_fSpeed = 1.f;
	m_fAngle = 0.f;
	m_vSpeed = { 0.f, 0.f, 0.f };
	m_fGravity = -9.8f;
	m_fAcmlTime = 0.f;
	m_iRcmState = _uint(CMonsterB2::B2S_SPAWN);

	// 공격 패턴 설정
	m_iDequeMinSize = 3;
	m_vecAtkPatterns.push_back({ CMonsterB2::B2S_SMASH, 40, true });
	m_vecAtkPatterns.push_back({ CMonsterB2::B2S_SHOOT, 40, true });
	m_vecAtkPatterns.push_back({ CMonsterB2::B2S_SUMMON, 40, true });

	// 시연용 : 모든 패턴이 순차적으로 실행
	m_patternDeque.push_back(CMonsterB2::B2S_SMASH);
	m_patternDeque.push_back(CMonsterB2::B2S_SHOOT);
	m_patternDeque.push_back(CMonsterB2::B2S_SUMMON);

	// 게임용 : 가중치와 난수를 통해 패턴을 채워줌
	Refill_Pattern();

	return S_OK;
}

void CB2_AI::Enter_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterB2::B2S_IDLE:
		break;

	case CMonsterB2::B2S_MOVESTART:
	{
		m_fSpeed = 0.05f;
		_vec3 vPrevPos, vDesiredDir;

		if ((!m_bChase) || (m_fAcmlTime < 2.f))			vDesiredDir = Randomize_Dir();
		else if ((m_bChase) && (m_fAcmlTime >= 2.f))	vDesiredDir = Compute_TargetDir();

		m_pOwnerTC->Get_Info(INFO_POS, &vPrevPos);
		m_vDir = Compute_LimitedDir(120.f, m_vDir, vDesiredDir);
		m_vLerpPos = vPrevPos + m_vDir * 3.f;
		m_vLerpPos.x += Get_Rand_Int(-5, 5) * 0.3f;	// -1.5f ~ 1.5f 난수
		m_vLerpPos.z += Get_Rand_Int(-5, 5) * 0.3f;	// -1.5f ~ 1.5f 난수
	}
	break;
	case CMonsterB2::B2S_MOVEEND:
	{
		m_vDir = Compute_TargetDir();
		m_vSpeed = { m_vDir.x * 3.f, 10.f, m_vDir.z * 3.f };
	}
	break;

	case CMonsterB2::B2S_HIT:
		break;

	case CMonsterB2::B2S_SMASH:
	{
		m_fAcmlTime = 0.f;
		m_fSpeed = 1.f;
	}
	break;

	case CMonsterB2::B2S_SHOOT:
	{
		m_fAcmlTime = 0.f;
		m_fSpeed = 0.3f;
		m_pOwnerTC->Get_Info(INFO_POS, &m_vLerpPos);
		m_vLerpPos -= m_vDir * 1.f;
	}
	break;

	case CMonsterB2::B2S_SUMMON:
	{
		m_fAcmlTime = 0.f;
		m_fSpeed = 0.1f;
		m_pOwnerTC->Get_Info(INFO_POS, &m_vLerpPos);
		m_vLerpPos += m_vDir * 15.f;
	}
	break;

	case CMonsterB2::B2S_SPAWN:
		m_fAcmlTime = 0.f;
		m_bOnce = true;
		break;

	case CMonsterB2::B2S_DIE:
		m_bActiveAI = false;
		break;

	case CMonsterB2::B2S_DEAD:
		break;
	}
}

void CB2_AI::Exit_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterB2::B2S_IDLE:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;

	case CMonsterB2::B2S_MOVESTART:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;

	case CMonsterB2::B2S_MOVEEND:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;

	case CMonsterB2::B2S_HIT:
		break;

	case CMonsterB2::B2S_SMASH:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;

	case CMonsterB2::B2S_SHOOT:
		break;

	case CMonsterB2::B2S_SUMMON:
		break;

	case CMonsterB2::B2S_SPAWN:
		m_bActiveAI = true;
		break;

	case CMonsterB2::B2S_DIE:
		break;

	case CMonsterB2::B2S_DEAD:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;
	}
}

void CB2_AI::Generate_Pattern(CMonsterB2::MONSTER_B2_STATE eLastPattern)
{
	_uint iTotalWeight(0);

	for (auto& pattern : m_vecAtkPatterns)
	{
		if (pattern.bIsActive && (pattern.eType != eLastPattern))
		{
			iTotalWeight += pattern.iWeight;
		}
	}

	_uint iRandom = Get_Rand_Int(1, iTotalWeight);
	_uint iAccumulated(0);

	for (auto& pattern : m_vecAtkPatterns)
	{
		if (!pattern.bIsActive || (pattern.eType == eLastPattern))
			continue;

		iAccumulated += pattern.iWeight;

		if (iRandom <= iAccumulated)
		{
			m_patternDeque.push_back(pattern.eType);
			break;
		}
	}
}

void CB2_AI::Refill_Pattern()
{
	while (m_patternDeque.size() < m_iDequeMinSize)
	{
		CMonsterB2::MONSTER_B2_STATE eLastState;

		if (m_patternDeque.empty()) eLastState = CMonsterB2::B2S_SUMMON;
		else						eLastState = m_patternDeque.back();

		Generate_Pattern(eLastState);
	}
}

_int CB2_AI::Update_Component(const _float& fTimeDelta)
{
	_int iExit(0);

	m_fAcmlTime += fTimeDelta;

	if (!m_bActiveAI) return iExit;

	Compute_Distance();

	switch (m_iCurState)
	{
	case CMonsterB2::B2S_IDLE:
		Update_Idle(fTimeDelta);
		break;
	case CMonsterB2::B2S_MOVESTART:
		Update_MoveStart(fTimeDelta);
		break;
	case CMonsterB2::B2S_MOVEEND:
		Update_MoveEnd(fTimeDelta);
		break;
	case CMonsterB2::B2S_HIT:
		Update_Hit(fTimeDelta);
		break;
	case CMonsterB2::B2S_SMASH:
		Update_Smash(fTimeDelta);
		break;
	case CMonsterB2::B2S_SHOOT:
		Update_Shoot(fTimeDelta);
		break;
	case CMonsterB2::B2S_SUMMON:
		Update_Summon(fTimeDelta);
		break;
	case CMonsterB2::B2S_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case CMonsterB2::B2S_DIE:
		Update_Die(fTimeDelta);
		break;
	case CMonsterB2::B2S_DEAD:
		Update_Dead(fTimeDelta);
		break;
	}

	Refill_Pattern();

	if (m_fAcmlTime >= 2.f)
	{
		Change_State(Get_Rand_Int(0, CMonsterB2::B2S_END));
		m_fAcmlTime = 0.f;
	}

	return iExit;
}

void CB2_AI::Update_Idle(const _float& fTimeDelta)
{
}

void CB2_AI::Update_MoveStart(const _float& fTimeDelta)
{
}

void CB2_AI::Update_MoveEnd(const _float& fTimeDelta)
{
}

void CB2_AI::Update_Hit(const _float& fTimeDelta)
{
}

void CB2_AI::Update_Smash(const _float& fTimeDelta)
{
}

void CB2_AI::Update_Shoot(const _float& fTimeDelta)
{
	if (m_bOnce)
	{
		if (m_pOwner)
			m_pOwner->Launch_Projectile(20);

		m_bOnce = false;
	}
}

void CB2_AI::Update_Summon(const _float& fTimeDelta)
{
	if (m_bOnce)
	{
		if (m_pOwner)
			m_pOwner->Summon_Minion(7);

		m_bOnce = false;
	}
}

void CB2_AI::Update_Spawn(const _float& fTimeDelta)
{
}

void CB2_AI::Update_Die(const _float& fTimeDelta)
{
}

void CB2_AI::Update_Dead(const _float& fTimeDelta)
{
}

void CB2_AI::Anim_End(CMonsterB2::MONSTER_B2_STATE eState)
{
	switch (eState)
	{
	case CMonsterB2::B2S_SPAWN:
		Change_State(CMonsterB2::B2S_IDLE);
		break;
	}
}

void CB2_AI::Push_Front_Pattern(CMonsterB2::MONSTER_B2_STATE eState)
{
	size_t iSize = m_vecAtkPatterns.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecAtkPatterns[i].eType == eState)
		{
			m_patternDeque.push_front(eState);
			return;
		}
	}
}

void CB2_AI::Set_Weight(CMonsterB2::MONSTER_B2_STATE eState, _uint iNewWeight)
{
	size_t iSize = m_vecAtkPatterns.size();

	for (size_t i = 0; i < iSize; ++i)
	{
		if (m_vecAtkPatterns[i].eType == eState)
		{
			m_vecAtkPatterns[i].iWeight = iNewWeight;

			if (iNewWeight == 0)	m_vecAtkPatterns[i].bIsActive = false;
			else					m_vecAtkPatterns[i].bIsActive = true;

			return;
		}
	}
}

CB2_AI* CB2_AI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	CB2_AI* pB2_AI = new CB2_AI(pGraphicDev);

	if (FAILED(pB2_AI->Ready_AI(fDetectRange, fInteractRange, iInitState)))
	{
		Safe_Release(pB2_AI);
		MSG_BOX("CB2_AI Create Failed");
		return nullptr;
	}

	return pB2_AI;
}

CComponent* CB2_AI::Clone()
{
	return new CB2_AI(*this);
}

void CB2_AI::Free()
{
}