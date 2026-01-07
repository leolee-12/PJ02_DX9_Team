#include "pch.h"
#include "CN2_AI.h"
#include "CTransform.h"

CN2_AI::CN2_AI(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CAIController(pGraphicDev),
		m_fSpeed(0.f),
		m_fAcmlTime(0.f),
		m_bChase(false),
		m_fAngle(0.f),
		m_fGravity(0.f),
		m_fGroundY(0.f)
{
}

CN2_AI::CN2_AI(const CN2_AI& rhs)
	:	CAIController(rhs),
		m_fSpeed(rhs.m_fSpeed),
		m_fAcmlTime(rhs.m_fAcmlTime),
		m_bChase(false),
		m_fAngle(rhs.m_fAngle),
		m_fGravity(rhs.m_fGravity),
		m_fGroundY(rhs.m_fGroundY)
{
}

CN2_AI::~CN2_AI()
{
}

HRESULT CN2_AI::Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	if (FAILED(CAIController::Ready_AI(fDetectRange, fInteractRange, iInitState)))
		return E_FAIL;
	
	m_fSpeed = 1.f;
	m_fAngle = 0.f;
	m_vSpeed = { 0.f, 0.f, 0.f};
	m_fGravity = -9.8f;
	m_fGroundY = 1.f;
	m_fAcmlTime = 0.f;
	m_iRcmState = _uint(CMonsterN2::N2S_SPAWN);

	return S_OK;
}

void CN2_AI::Enter_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterN2::N2S_CRAWL:
	{
		m_fSpeed = 0.03f;
		_vec3 vPrevPos, vDesiredDir;
		
		if		((!m_bChase) || (m_fAcmlTime < 2.f))	vDesiredDir = Randomize_Dir();
		else if ((m_bChase) && (m_fAcmlTime >= 2.f))	vDesiredDir = Compute_TargetDir();

		m_pOwnerTC->Get_Info(INFO_POS, &vPrevPos);
		m_vDir = Compute_LimitedDir(60.f, m_vDir, vDesiredDir);
		m_vLerpPos = vPrevPos + m_vDir * 3.f;
		m_vLerpPos.x += Get_Rand_Int(-5, 5) * 0.3f;	// -1.5f ~ 1.5f 난수
		m_vLerpPos.z += Get_Rand_Int(-5, 5) * 0.3f;	// -1.5f ~ 1.5f 난수
	}
		break;
	case CMonsterN2::N2S_JUMP:
	{
		m_vDir = Compute_TargetDir();
		m_vSpeed = { m_vDir.x * 3.f, 5.f, m_vDir.z * 3.f };
	}
		break;
	case CMonsterN2::N2S_LAND:
	{
		m_fAcmlTime = 0.f;
		m_fSpeed = 1.f;
	}
		break;
	case CMonsterN2::N2S_SPAWN:
		m_bActiveAI = false;
		break;
	case CMonsterN2::N2S_STOP:
		break;
	}
}

void CN2_AI::Exit_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterN2::N2S_CRAWL:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;

	case CMonsterN2::N2S_JUMP:
	{
	}
	break;

	case CMonsterN2::N2S_LAND:
	{
		if (!m_pTargetTC) m_bChase = false;
		m_vSpeed = { 0.f, 0.f, 0.f };
	}
	break;

	case CMonsterN2::N2S_SPAWN:
		m_bActiveAI = true;
		break;

	case CMonsterN2::N2S_STOP:
	{
		if (!m_pTargetTC) m_bChase = false;
	}
	break;
	}
}

_int CN2_AI::Update_Component(const _float& fTimeDelta)
{
	_int iExit(0);

	m_fAcmlTime += fTimeDelta;

	if (!m_bActiveAI) return iExit;

	Compute_Distance();

	switch (m_iCurState)
	{
	case CMonsterN2::N2S_CRAWL:
		Update_Crawl(fTimeDelta);
		break;
	case CMonsterN2::N2S_JUMP:
		Update_Jump(fTimeDelta);
		break;
	case CMonsterN2::N2S_LAND:
		Update_Land(fTimeDelta);
		break;
	case CMonsterN2::N2S_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case CMonsterN2::N2S_STOP:
		Update_Stop(fTimeDelta);
		break;
	}

	return iExit;
}

void CN2_AI::Update_Crawl(const _float& fTimeDelta)
{
	if (m_bChase)
	{	// 타겟을 이미 발견했을 때
		if (m_fDistance <= m_fInteractRange)
		{
			if (m_fAcmlTime >= 5.f)
				Change_State(CMonsterN2::N2S_JUMP);
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

void CN2_AI::Update_Jump(const _float& fTimeDelta)
{
	m_vSpeed.y += m_fGravity * fTimeDelta;
	m_pOwnerTC->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);

	_vec3 vPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vPos);
	if (vPos.y < m_fGroundY)
	{
		m_pOwnerTC->Set_Pos(vPos.x, m_fGroundY, vPos.z);
		Change_State(CMonsterN2::N2S_LAND);
	}
}

void CN2_AI::Update_Land(const _float& fTimeDelta)
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

void CN2_AI::Update_Spawn(const _float& fTimeDelta)
{
}

void CN2_AI::Update_Stop(const _float& fTimeDelta)
{
	if ((!m_bChase) && (m_fDistance <= m_fDetectRange))
	{
		m_bChase = true;
	}
	else if ((m_bChase) && (m_fDistance <= m_fInteractRange) && (m_fAcmlTime >= 5.f))
	{
		Change_State(CMonsterN2::N2S_JUMP);
		return;
	}

	Change_State(CMonsterN2::N2S_CRAWL);
}

void CN2_AI::Anim_End(CMonsterN2::MONSTER_N2_STATE eState)
{
	switch (eState)
	{
	case CMonsterN2::N2S_CRAWL:
		Change_State(CMonsterN2::N2S_STOP);
		break;

	case CMonsterN2::N2S_JUMP:
		Change_State(CMonsterN2::N2S_LAND);
		break;

	case CMonsterN2::N2S_LAND:
		Change_State(CMonsterN2::N2S_CRAWL);
		break;

	case CMonsterN2::N2S_SPAWN:
		Change_State(CMonsterN2::N2S_CRAWL);
		break;
	}
}

CN2_AI* CN2_AI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	CN2_AI* pN2_AI = new CN2_AI(pGraphicDev);

	if (FAILED(pN2_AI->Ready_AI(fDetectRange, fInteractRange, iInitState)))
	{
		Safe_Release(pN2_AI);
		MSG_BOX("CN2_AI Create Failed");
		return nullptr;
	}

	return pN2_AI;
}

CComponent* CN2_AI::Clone()
{
	return new CN2_AI(*this);
}

void CN2_AI::Free()
{
}