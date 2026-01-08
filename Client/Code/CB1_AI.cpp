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
	m_fGroundY(0.f)
{
}

CB1_AI::CB1_AI(const CB1_AI& rhs)
	: CAIController(rhs),
	m_fSpeed(rhs.m_fSpeed),
	m_fAcmlTime(rhs.m_fAcmlTime),
	m_bChase(false),
	m_fAngle(rhs.m_fAngle),
	m_fGravity(rhs.m_fGravity),
	m_fGroundY(rhs.m_fGroundY)
{
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
	m_fGroundY = 1.f;
	m_fAcmlTime = 0.f;
	m_iRcmState = _uint(CMonsterB1::B1S_SPAWN);

	return S_OK;
}

void CB1_AI::Enter_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterB1::B1S_CRAWL:
	{
		m_fSpeed = 0.03f;
		_vec3 vPrevPos, vDesiredDir;

		if ((!m_bChase) || (m_fAcmlTime < 2.f))	vDesiredDir = Randomize_Dir();
		else if ((m_bChase) && (m_fAcmlTime >= 2.f))	vDesiredDir = Compute_TargetDir();

		m_pOwnerTC->Get_Info(INFO_POS, &vPrevPos);
		m_vDir = Compute_LimitedDir(60.f, m_vDir, vDesiredDir);
		m_vLerpPos = vPrevPos + m_vDir * 3.f;
		m_vLerpPos.x += Get_Rand_Int(-5, 5) * 0.3f;	// -1.5f ~ 1.5f 난수
		m_vLerpPos.z += Get_Rand_Int(-5, 5) * 0.3f;	// -1.5f ~ 1.5f 난수
	}
	break;
	case CMonsterB1::B1S_JUMP:
	{
		m_vDir = Compute_TargetDir();
		m_vSpeed = { m_vDir.x * 3.f, 5.f, m_vDir.z * 3.f };
	}
	break;
	case CMonsterB1::B1S_LAND:
	{
		m_fAcmlTime = 0.f;
		m_fSpeed = 1.f;
	}
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

	case CMonsterB1::B1S_SPAWN:
		m_bActiveAI = true;
		break;

	case CMonsterB1::B1S_STOP:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;
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
	case CMonsterB1::B1S_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case CMonsterB1::B1S_STOP:
		Update_Stop(fTimeDelta);
		break;
	}

	return iExit;
}

void CB1_AI::Update_Crawl(const _float& fTimeDelta)
{
	if (m_bChase)
	{	// 타겟을 이미 발견했을 때
		if (m_fDistance <= m_fInteractRange)
		{
			if (m_fAcmlTime >= 5.f)
				Change_State(CMonsterB1::B1S_JUMP);
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
		Change_State(CMonsterB1::B1S_JUMP);
		return;
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

	case CMonsterB1::B1S_JUMP:
		Change_State(CMonsterB1::B1S_LAND);
		break;

	case CMonsterB1::B1S_LAND:
		Change_State(CMonsterB1::B1S_CRAWL);
		break;

	case CMonsterB1::B1S_SPAWN:
		Change_State(CMonsterB1::B1S_CRAWL);
		break;
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