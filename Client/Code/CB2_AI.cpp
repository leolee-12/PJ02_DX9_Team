#include "pch.h"
#include "CB2_AI.h"
#include "CTransform.h"

CB2_AI::CB2_AI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CAIController(pGraphicDev),
	m_fSpeed(0.f),
	m_fAcmlTime(0.f),
	m_bChase(false),
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
	m_vSpeed = { 0.f, 0.f, 0.f };
	m_fAcmlTime = 0.f;
	m_iRcmState = _uint(CMonsterB2::B2S_SPAWN);

	// 공격 패턴 설정
	m_iDequeMinSize = 3;
	m_vecAtkPatterns.push_back({ CMonsterB2::B2S_SMASH,	 2, true });
	m_vecAtkPatterns.push_back({ CMonsterB2::B2S_SHOOT,	 2, true });
	m_vecAtkPatterns.push_back({ CMonsterB2::B2S_SUMMON, 2, true });
	m_vecAtkPatterns.push_back({ CMonsterB2::B2S_SPIKE1, 0, false });
	m_vecAtkPatterns.push_back({ CMonsterB2::B2S_SPIKE2, 0, false });

	// 시연용 : 모든 패턴이 순차적으로 실행
	m_patternDeque.push_back(CMonsterB2::B2S_SMASH);
	m_patternDeque.push_back(CMonsterB2::B2S_SHOOT);
	m_patternDeque.push_back(CMonsterB2::B2S_SUMMON);

	// 게임용 : 가중치와 난수를 통해 패턴을 채워줌
	Refill_Pattern(true);

	return S_OK;
}

void CB2_AI::Enter_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterB2::B2S_IDLE:
		break;

	case CMonsterB2::B2S_DIG:
	{
		m_fAcmlTime = 0.f;

		m_fSpeed = 0.03f;
		_vec3 vPrevPos, vDesiredDir;
		m_pOwnerTC->Get_Info(INFO_POS, &vPrevPos);
		vDesiredDir = Compute_TargetDir();

		m_vDir = Compute_LimitedDir(120.f, m_vDir, vDesiredDir);
		m_vLerpPos = vPrevPos + m_vDir * 13.f;
	}
	break;
	case CMonsterB2::B2S_ESCAPE:
	{
		m_vDir = Compute_TargetDir();
		m_vSpeed = { m_vDir.x * 3.f, 10.f, m_vDir.z * 3.f };
	}
	break;

	case CMonsterB2::B2S_HIT:
		break;

	case CMonsterB2::B2S_SMASH:
		break;

	case CMonsterB2::B2S_SHOOT:
		break;

	case CMonsterB2::B2S_SUMMON:
		break;

	case CMonsterB2::B2S_SPAWN:
		break;

	case CMonsterB2::B2S_DIE:
		break;

	case CMonsterB2::B2S_DEAD:
		break;

	case CMonsterB2::B2S_JUMP:
		break;

	case CMonsterB2::B2S_DIVE:
		break;

	case CMonsterB2::B2S_SPIKE1:
		break;

	case CMonsterB2::B2S_SPIKE2:
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

	case CMonsterB2::B2S_DIG:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;

	case CMonsterB2::B2S_ESCAPE:
	{
		if (!m_pTargetTC) m_bChase = false;
		m_fAcmlTime = 0.f;
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

	case CMonsterB2::B2S_JUMP:
		break;

	case CMonsterB2::B2S_DIVE:
		break;

	case CMonsterB2::B2S_SPIKE1:
		break;

	case CMonsterB2::B2S_SPIKE2:
		break;
	}
}

void CB2_AI::Generate_Pattern(CMonsterB2::MONSTER_B2_STATE eLastPattern, _bool bAllowDuplicate)
{
	size_t iPatternCnt = m_vecAtkPatterns.size();

	if (iPatternCnt == 0)	// error : 등록된 공격 패턴이 없음
	{
		m_patternDeque.push_back(CMonsterB2::MONSTER_B2_STATE(0));
		return;
	}

	_uint iTotalWeight(0);

	if (bAllowDuplicate || iPatternCnt == 1)
	{
		for (auto& pattern : m_vecAtkPatterns)
		{
			if (pattern.bIsActive)
				iTotalWeight += pattern.iWeight;
		}
	}
	else
	{
		for (auto& pattern : m_vecAtkPatterns)
		{
			if (pattern.bIsActive && (pattern.eType != eLastPattern))
				iTotalWeight += pattern.iWeight;
		}
	}

	if (iTotalWeight == 0)	// error : 활성화된 공격 패턴이 없음
	{
		m_patternDeque.push_back(CMonsterB2::MONSTER_B2_STATE(0));
		return;
	}

	_uint iRandom = Get_Rand_Int(1, iTotalWeight);
	_uint iAccumulated(0);

	for (auto& pattern : m_vecAtkPatterns)
	{
		if (!pattern.bIsActive) continue;

		if (!bAllowDuplicate && iPatternCnt > 1 && pattern.eType == eLastPattern)
			continue;

		iAccumulated += pattern.iWeight;

		if (iRandom <= iAccumulated)
		{
			// 정상 생성
			m_patternDeque.push_back(pattern.eType);
			return;
		}
	}

	// error : 정상적으로 생성되지 않음
	m_patternDeque.push_back(CMonsterB2::MONSTER_B2_STATE(0));
}

void CB2_AI::Refill_Pattern(_bool bAllowDuplicate)
{
	while (m_patternDeque.size() < m_iDequeMinSize)
	{
		CMonsterB2::MONSTER_B2_STATE eLastState;

		if (m_patternDeque.empty()) eLastState = CMonsterB2::B2S_SUMMON;
		else						eLastState = m_patternDeque.back();

		Generate_Pattern(eLastState, bAllowDuplicate);
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
	case CMonsterB2::B2S_DIG:
		Update_Dig(fTimeDelta);
		break;
	case CMonsterB2::B2S_ESCAPE:
		Update_Escape(fTimeDelta);
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
	case CMonsterB2::B2S_JUMP:
		Update_Jump(fTimeDelta);
		break;
	case CMonsterB2::B2S_DIVE:
		Update_Dive(fTimeDelta);
		break;
	case CMonsterB2::B2S_SPIKE1:
		Update_Spike1(fTimeDelta);
		break;
	case CMonsterB2::B2S_SPIKE2:
		Update_Spike2(fTimeDelta);
		break;
	}

	Refill_Pattern();

	return iExit;
}

void CB2_AI::Update_Idle(const _float& fTimeDelta)
{
	if (!m_bChase)
	{	// 타겟을 발견하지 못했을 때
		if (m_fDistance <= m_fDetectRange)
		{	// 타겟이 감지 범위 내로 진입 시 발견
			m_bChase = true;
		}
	}
}

void CB2_AI::Update_Dig(const _float& fTimeDelta)
{
	if (m_fAcmlTime >= 5.f) Change_State(CMonsterB2::B2S_ESCAPE);
	else if (m_fAcmlTime >= 1.5f)
	{
		_vec3 vPos;
		m_pOwnerTC->Get_Info(INFO_POS, &vPos);
		D3DXVec3Lerp(&vPos, &vPos, &m_vLerpPos, m_fSpeed);
		m_pOwnerTC->Set_Pos(vPos.x, vPos.y, vPos.z);
	}

	if (m_bOnce)
	{
		if (m_pOwner)
		{
			m_pOwner->Summon_Spike(1);
		}
		m_bOnce = false;
	}
}

void CB2_AI::Update_Escape(const _float& fTimeDelta)
{
}

void CB2_AI::Update_Hit(const _float& fTimeDelta)
{
	if (m_fAcmlTime >= 3.f)
	{
		if (!m_patternDeque.empty())
		{
			Change_State(m_patternDeque.front());
			m_patternDeque.pop_front();
		}
	}
}

void CB2_AI::Update_Smash(const _float& fTimeDelta)
{
	if (m_bOnce)
	{
		if (m_pOwner)
		{
			_vec3 vPos;
			m_pOwnerTC->Get_Info(INFO_POS, &vPos);
			vPos.y += 2.5f;

			switch (m_iSwitch)
			{
			case 1:
				vPos.x -= 5.f;
				m_pOwner->Attack_HitBox(vPos);
				break;

			case 2:
				vPos.x += 5.f;
				m_pOwner->Attack_HitBox(vPos);
				break;

			case 3:
				m_pOwner->Attack_HitBox(vPos);
				break;
			}
		}

		m_bOnce = false;
	}
}

void CB2_AI::Update_Shoot(const _float& fTimeDelta)
{
	if (m_bOnce)
	{
		if (m_pOwner)
			m_pOwner->Launch_Projectile(10, Compute_TargetDir());

		m_bOnce = false;
	}
}

void CB2_AI::Update_Summon(const _float& fTimeDelta)
{
	if (m_bOnce)
	{
		if (m_pOwner)
			m_pOwner->Summon_Minion(3);

		m_bOnce = false;
	}
}

void CB2_AI::Update_Spawn(const _float& fTimeDelta)
{	// Empty
}

void CB2_AI::Update_Die(const _float& fTimeDelta)
{	// Empty
}

void CB2_AI::Update_Dead(const _float& fTimeDelta)
{	// Empty
}

void CB2_AI::Update_Jump(const _float& fTimeDelta)
{
}

void CB2_AI::Update_Dive(const _float& fTimeDelta)
{
}

void CB2_AI::Update_Spike1(const _float& fTimeDelta)
{
	if (m_bOnce)
	{
		if (m_pOwner)
		{
			_float fSpeed = 8.f;
			_float fRadian = 2.f * D3DX_PI * 0.2f;

			for (_float f = 0.f; f < 5.f; f += 1.f)
			{
				m_pOwner->Summon_Spike(10, _vec3{ fSpeed * cosf(f * fRadian),	0.f, fSpeed * sinf(f * fRadian) });
			}
		}
		m_bOnce = false;
	}
}

void CB2_AI::Update_Spike2(const _float& fTimeDelta)
{
	if (m_bOnce)
	{
		if (m_pOwner)
		{
			_float fSpeed = 6.f;
			_float fRadian = 0.f;
			_float fGap = 2.f * D3DX_PI * 0.05f;

			for (int i = 0; i < 20; ++i)
			{
				m_pOwner->Summon_Spike(5, _vec3{ fSpeed * cosf(fRadian), 0.f, fSpeed * sinf(fRadian) });
				fRadian += fGap;
			}
		}
		m_bOnce = false;
	}
}

void CB2_AI::Anim_End(CMonsterB2::MONSTER_B2_STATE eState)
{
	switch (eState)
	{
	case CMonsterB2::B2S_IDLE:
	{
		if ((m_bChase) && (m_fDistance <= m_fInteractRange))
		{
			if (!m_patternDeque.empty())
			{
				Change_State(m_patternDeque.front());
				m_patternDeque.pop_front();
			}
		}
	}
	break;

	case CMonsterB2::B2S_ESCAPE:
	case CMonsterB2::B2S_HIT:
	case CMonsterB2::B2S_SPAWN:
		Change_State(CMonsterB2::B2S_IDLE);
		break;

	case CMonsterB2::B2S_SMASH:
	case CMonsterB2::B2S_SHOOT:
	case CMonsterB2::B2S_SUMMON:
	case CMonsterB2::B2S_SPIKE1:
	case CMonsterB2::B2S_SPIKE2:
		Change_State(CMonsterB2::B2S_DIG);
		break;

	case CMonsterB2::B2S_DIE:
		Change_State(CMonsterB2::B2S_DEAD);
		break;

	case CMonsterB2::B2S_DIVE:
		Change_State(CMonsterB2::B2S_ESCAPE);
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

void CB2_AI::Set_Signal(_uint iNum)
{
	m_bOnce = true;

	if (iNum != 0) m_iSwitch = iNum;
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
