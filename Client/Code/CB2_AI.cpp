#include "pch.h"
#include "CB1_AI.h"
#include "CTransform.h"

CB1_AI::CB1_AI(LPDIRECT3DDEVICE9 pGraphicDev)
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

CB1_AI::CB1_AI(const CB1_AI& rhs)
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

CB1_AI::~CB1_AI()
{
}

HRESULT CB1_AI::Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	if (FAILED(CAIController::Ready_AI(fDetectRange, fInteractRange, iInitState)))
		return E_FAIL;

	m_fSpeed = 1.f;
	m_fAngle = 0.f;
	m_vSpeed = { 0.f, 0.f, 0.f };
	m_fGravity = -9.8f;
	m_fAcmlTime = 0.f;
	m_iRcmState = _uint(CMonsterB1::B1S_SPAWN);

	// 공격 패턴 설정
	m_iDequeMinSize = 3;
	m_vecAtkPatterns.push_back({ CMonsterB1::B1S_JUMP, 40, true });
	m_vecAtkPatterns.push_back({ CMonsterB1::B1S_PREPARE, 40, true });
	m_vecAtkPatterns.push_back({ CMonsterB1::B1S_SHOOT, 40, true });
	m_vecAtkPatterns.push_back({ CMonsterB1::B1S_SUMMON, 20, true });

	// 시연용 : 모든 패턴이 순차적으로 실행
	m_patternDeque.push_back(CMonsterB1::B1S_JUMP);
	m_patternDeque.push_back(CMonsterB1::B1S_PREPARE);
	m_patternDeque.push_back(CMonsterB1::B1S_SHOOT);
	m_patternDeque.push_back(CMonsterB1::B1S_SUMMON);

	// 게임용 : 가중치와 난수를 통해 패턴을 채워줌
	Refill_Pattern();

	return S_OK;
}

void CB1_AI::Enter_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterB1::B1S_CRAWL:
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
	case CMonsterB1::B1S_JUMP:
	{
		m_vDir = Compute_TargetDir();
		m_vSpeed = { m_vDir.x * 3.f, 10.f, m_vDir.z * 3.f };
	}
	break;
	case CMonsterB1::B1S_LAND:
	{
		m_fAcmlTime = 0.f;
		m_fSpeed = 1.f;
	}
	break;

	case CMonsterB1::B1S_PREPARE:
	{
		m_fAcmlTime = 0.f;
		m_fSpeed = 0.3f;
		m_pOwnerTC->Get_Info(INFO_POS, &m_vLerpPos);
		m_vLerpPos -= m_vDir * 1.f;
	}
	break;

	case CMonsterB1::B1S_ATTACK:
	{
		m_fAcmlTime = 0.f;
		m_fSpeed = 0.1f;
		m_pOwnerTC->Get_Info(INFO_POS, &m_vLerpPos);
		m_vLerpPos += m_vDir * 15.f;
	}
	break;

	case CMonsterB1::B1S_SHOOT:
		m_fAcmlTime = 0.f;
		m_bOnce = true;
		break;

	case CMonsterB1::B1S_SUMMON:
		m_fAcmlTime = 0.f;
		m_bOnce = true;
		break;

	case CMonsterB1::B1S_ROAR:
		break;

	case CMonsterB1::B1S_SPAWN:
		m_bActiveAI = false;
		break;

	case CMonsterB1::B1S_STOP:
		break;
	}
}

void CB1_AI::Exit_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterB1::B1S_CRAWL:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;

	case CMonsterB1::B1S_JUMP:
	{
	}
	break;

	case CMonsterB1::B1S_LAND:
	{
		if (!m_pTargetTC) m_bChase = false;
		m_vSpeed = { 0.f, 0.f, 0.f };
	}
	break;

	case CMonsterB1::B1S_PREPARE:
		break;

	case CMonsterB1::B1S_ATTACK:
		break;

	case CMonsterB1::B1S_SUMMON:
		break;

	case CMonsterB1::B1S_ROAR:
		m_bActiveAI = true;
		break;

	case CMonsterB1::B1S_SPAWN:
		break;

	case CMonsterB1::B1S_STOP:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;
	}
}

void CB1_AI::Generate_Pattern(CMonsterB1::MONSTER_B1_STATE eLastPattern)
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

void CB1_AI::Refill_Pattern()
{
	while (m_patternDeque.size() < m_iDequeMinSize)
	{
		CMonsterB1::MONSTER_B1_STATE eLastState;

		if (m_patternDeque.empty()) eLastState = CMonsterB1::B1S_SUMMON;
		else						eLastState = m_patternDeque.back();

		Generate_Pattern(eLastState);
	}
}

_int CB1_AI::Update_Component(const _float& fTimeDelta)
{
	_int iExit(0);

	m_fAcmlTime += fTimeDelta;

	if (!m_bActiveAI) return iExit;

	Compute_Distance();

	switch (m_iCurState)
	{
	case CMonsterB1::B1S_CRAWL:
		Update_Crawl(fTimeDelta);
		break;
	case CMonsterB1::B1S_JUMP:
		Update_Jump(fTimeDelta);
		break;
	case CMonsterB1::B1S_LAND:
		Update_Land(fTimeDelta);
		break;
	case CMonsterB1::B1S_PREPARE:
		Update_Prepare(fTimeDelta);
		break;
	case CMonsterB1::B1S_ATTACK:
		Update_Attack(fTimeDelta);
		break;
	case CMonsterB1::B1S_SHOOT:
		Update_Shoot(fTimeDelta);
		break;
	case CMonsterB1::B1S_SUMMON:
		Update_Summon(fTimeDelta);
		break;
	case CMonsterB1::B1S_ROAR:
		Update_Roar(fTimeDelta);
		break;
	case CMonsterB1::B1S_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case CMonsterB1::B1S_STOP:
		Update_Stop(fTimeDelta);
		break;
	}

	Refill_Pattern();

	return iExit;
}

void CB1_AI::Update_Crawl(const _float& fTimeDelta)
{
	if (m_bChase)
	{	// 타겟을 이미 발견했을 때
		if (m_fDistance <= m_fInteractRange)
		{
			if (m_fAcmlTime >= 5.f)
			{
				if (!m_patternDeque.empty())
				{
					Change_State(m_patternDeque.front());
					m_patternDeque.pop_front();
				}
			}
		}
	}
	else
	{	// 타겟을 발견하지 못했을 때
		if (m_fDistance <= m_fDetectRange)
		{	// 타겟이 감지 범위 내로 진입 시 발견
			m_bChase = true;
		}
	}

	//m_pOwnerTC->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
	_vec3 vPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vPos);
	D3DXVec3Lerp(&vPos, &vPos, &m_vLerpPos, m_fSpeed);
	m_pOwnerTC->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CB1_AI::Update_Jump(const _float& fTimeDelta)
{
	m_vSpeed.y += m_fGravity * fTimeDelta;
	m_pOwnerTC->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);

	_vec3 vPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vPos);
	if (vPos.y < m_fGroundY)
	{
		m_pOwnerTC->Set_Pos(vPos.x, m_fGroundY, vPos.z);
		Change_State(CMonsterB1::B1S_LAND);
	}
}

void CB1_AI::Update_Land(const _float& fTimeDelta)
{
	if (m_fAcmlTime < 0.2f)  // 0.2초 동안
	{
		_vec3 vPos;
		m_pOwnerTC->Get_Info(INFO_POS, &vPos);
		_float fDeceleration = 1.0f - (m_fAcmlTime / 0.2f);
		vPos += m_vDir * 0.5f * fDeceleration * fTimeDelta;
		m_pOwnerTC->Set_Pos(vPos.x, vPos.y, vPos.z);
	}
}

void CB1_AI::Update_Prepare(const _float& fTimeDelta)
{
	_vec3 vDesiredDir = Compute_TargetDir();
	m_vDir = Compute_LimitedDir(60.f * fTimeDelta, m_vDir, vDesiredDir);

	_vec3 vPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vPos);
	D3DXVec3Lerp(&vPos, &vPos, &m_vLerpPos, m_fSpeed);
	m_pOwnerTC->Set_Pos(vPos.x, vPos.y, vPos.z);

	if (m_fAcmlTime >= 2.f) Change_State(CMonsterB1::B1S_ATTACK);
}

void CB1_AI::Update_Attack(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vPos);
	D3DXVec3Lerp(&vPos, &vPos, &m_vLerpPos, m_fSpeed);
	m_pOwnerTC->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CB1_AI::Update_Shoot(const _float& fTimeDelta)
{
	if (m_bOnce)
	{
		if (m_pOwner)
			m_pOwner->Launch_Projectile(20);

		m_bOnce = false;
	}
}

void CB1_AI::Update_Summon(const _float& fTimeDelta)
{
	if (m_bOnce)
	{
		if (m_pOwner)
			m_pOwner->Summon_Minion(7);

		m_bOnce = false;
	}
}

void CB1_AI::Update_Roar(const _float& fTimeDelta)
{
}

void CB1_AI::Update_Spawn(const _float& fTimeDelta)
{
}

void CB1_AI::Update_Stop(const _float& fTimeDelta)
{
	if ((!m_bChase) && (m_fDistance <= m_fDetectRange))
	{
		m_bChase = true;
	}
	else if ((m_bChase) && (m_fDistance <= m_fInteractRange) && (m_fAcmlTime >= 5.f))
	{
		if (!m_patternDeque.empty())
		{
			Change_State(m_patternDeque.front());
			m_patternDeque.pop_front();
			return;
		}
	}

	Change_State(CMonsterB1::B1S_CRAWL);
}

void CB1_AI::Anim_End(CMonsterB1::MONSTER_B1_STATE eState)
{
	switch (eState)
	{
	case CMonsterB1::B1S_CRAWL:
		Change_State(CMonsterB1::B1S_STOP);
		break;

	case CMonsterB1::B1S_LAND:
		Change_State(CMonsterB1::B1S_CRAWL);
		break;

	case CMonsterB1::B1S_PREPARE:
		Change_State(CMonsterB1::B1S_ATTACK);
		break;

	case CMonsterB1::B1S_ATTACK:
		Change_State(CMonsterB1::B1S_CRAWL);
		break;

	case CMonsterB1::B1S_SHOOT:
		Change_State(CMonsterB1::B1S_CRAWL);
		break;

	case CMonsterB1::B1S_SUMMON:
		Change_State(CMonsterB1::B1S_CRAWL);
		break;

	case CMonsterB1::B1S_ROAR:
		Change_State(CMonsterB1::B1S_CRAWL);
		break;

	case CMonsterB1::B1S_SPAWN:
		Change_State(CMonsterB1::B1S_ROAR);
		break;
	}
}

void CB1_AI::Push_Front_Pattern(CMonsterB1::MONSTER_B1_STATE eState)
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

void CB1_AI::Set_Weight(CMonsterB1::MONSTER_B1_STATE eState, _uint iNewWeight)
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

CB1_AI* CB1_AI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	CB1_AI* pB1_AI = new CB1_AI(pGraphicDev);

	if (FAILED(pB1_AI->Ready_AI(fDetectRange, fInteractRange, iInitState)))
	{
		Safe_Release(pB1_AI);
		MSG_BOX("CB1_AI Create Failed");
		return nullptr;
	}

	return pB1_AI;
}

CComponent* CB1_AI::Clone()
{
	return new CB1_AI(*this);
}

void CB1_AI::Free()
{
}