#include "pch.h"
#include "CN3_AI.h"
#include "CTransform.h"

CN3_AI::CN3_AI(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CAIController(pGraphicDev),
		m_fSpeed(0.f),
		m_fAccTime(0.f),
		m_bChase(false)
{
}

CN3_AI::CN3_AI(const CN3_AI& rhs)
	:	CAIController(rhs),
		m_fSpeed(rhs.m_fSpeed),
		m_fAccTime(rhs.m_fAccTime),
		m_bChase(false)
{
}

CN3_AI::~CN3_AI()
{
}

HRESULT CN3_AI::Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	if (FAILED(CAIController::Ready_AI(fDetectRange, fInteractRange, iInitState)))
		return E_FAIL;

	m_fSpeed = 0.5f;
	m_fAccTime = 0.f;
	m_iRcmState = _uint(CMonsterN3::N3S_FLY);

	return S_OK;
}

void CN3_AI::Enter_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterN3::N3S_FLY:
	{
		_vec3 vPrevPos, vDesiredDir;

		if (!m_bChase)
		{
			m_fSpeed = 1.5f;

			if (m_fAccTime < 2.f) vDesiredDir = Randomize_Dir();
		}
		else
		{
			m_fSpeed = 3.f;
			if (m_fAccTime >= 2.f) vDesiredDir = Compute_TargetDir();
		}
		m_pOwnerTC->Get_Info(INFO_POS, &vPrevPos);
		m_vDir = Compute_LimitedDir(60.f, m_vDir, vDesiredDir);
		m_vDir.x += Get_Rand_Int(-5, 5) * 0.05f;	// -0.25f ~ 0.25f 난수
		m_vDir.z += Get_Rand_Int(-5, 5) * 0.05f;	// -0.25f ~ 0.25f 난수
		m_vDir.y = 0.f;
		D3DXVec3Normalize(&m_vDir, &m_vDir);
	}
		break;
	case CMonsterN3::N3S_PREPARE:
	{
		m_fSpeed = 0.3f;
		m_pOwnerTC->Get_Info(INFO_POS, &m_vLerpPos);
		m_vLerpPos -= m_vDir * 1.f;
	}
		break;
	case CMonsterN3::N3S_RUSH:
	{
		m_fSpeed = 0.3f;
		m_pOwnerTC->Get_Info(INFO_POS, &m_vLerpPos);
		m_vLerpPos += m_vDir * 5.f;
	}
		break;
	case CMonsterN3::N3S_SPAWN:
		m_bActiveAI = false;
		break;
	case CMonsterN3::N3S_STOP:
		break;
	}
}

void CN3_AI::Exit_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterN3::N3S_FLY:
		if (!m_pTargetTC) m_bChase = false;
		break;
	case CMonsterN3::N3S_PREPARE:
		if (!m_pTargetTC) m_bChase = false;
		break;
	case CMonsterN3::N3S_RUSH:
		if (!m_pTargetTC) m_bChase = false;
		m_fAccTime = 0.f;
		break;
	case CMonsterN3::N3S_SPAWN:
		break;
	case CMonsterN3::N3S_STOP:
		if (!m_pTargetTC) m_bChase = false;
		break;
	}
}

_int CN3_AI::Update_Component(const _float& fTimeDelta)
{
	_int iExit(0);

	m_fAccTime += fTimeDelta;

	Compute_Distance();

	if (!m_bActiveAI)
	{
		if (m_fDistance <= m_fDetectRange)	m_bActiveAI = true;
		else								return iExit;
	}

	switch (m_iCurState)
	{
	case CMonsterN3::N3S_FLY:
		Update_Fly(fTimeDelta);
		break;
	case CMonsterN3::N3S_PREPARE:
		Update_Prepare(fTimeDelta);
		break;
	case CMonsterN3::N3S_RUSH:
		Update_Rush(fTimeDelta);
		break;
	case CMonsterN3::N3S_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	case CMonsterN3::N3S_STOP:
		Update_Stop(fTimeDelta);
		break;
	}

	return iExit;
}

void CN3_AI::Update_Fly(const _float& fTimeDelta)
{
	if (!m_pTargetTC) return;

	if (m_bChase)
	{	// 타겟을 이미 발견했을 때
		if (m_fDistance <= m_fInteractRange)
		{
			if (m_fAccTime >= 5.f)
				Change_State(CMonsterN3::N3S_PREPARE);
		}
	}
	else
	{	// 타겟을 발견하지 못했을 때
		if (m_fDistance <= m_fDetectRange)
		{	// 타겟이 감지 범위 내로 진입 시 발견
			m_bChase = true;
		}
	}

	m_pOwnerTC->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);

	if (_uint(m_fAccTime) % 3 == 0)
	{	// 타겟이 감지 범위 내에 없을 때는 순찰하다 대기 상태로 전환
		Change_State(CMonsterN3::N3S_STOP);
		return;
	}
}

void CN3_AI::Update_Prepare(const _float& fTimeDelta)
{
	_vec3 vDesiredDir = Compute_TargetDir();
	m_vDir = Compute_LimitedDir(60.f * fTimeDelta, m_vDir, vDesiredDir);

	_vec3 vPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vPos);
	D3DXVec3Lerp(&vPos, &vPos, &m_vLerpPos, m_fSpeed);
	m_pOwnerTC->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CN3_AI::Update_Rush(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vPos);
	D3DXVec3Lerp(&vPos, &vPos, &m_vLerpPos, m_fSpeed);
	m_pOwnerTC->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CN3_AI::Update_Spawn(const _float& fTimeDelta)
{	// Empty
}

void CN3_AI::Update_Stop(const _float& fTimeDelta)
{
	if ((!m_bChase) && (m_fDistance <= m_fDetectRange))
	{
		m_bChase = true;
	}
	else if ((m_bChase) && (m_fDistance <= m_fInteractRange) && (m_fAccTime >= 5.f))
	{
		Change_State(CMonsterN3::N3S_PREPARE);
		return;
	}

	if (_uint(m_fAccTime) % 3 != 0)
	{	// 타겟이 감지 범위 내에 없을 때는 순찰하다 대기 상태로 전환
		Change_State(CMonsterN3::N3S_FLY);
	}
}

void CN3_AI::Anim_End(CMonsterN3::MONSTER_N3_STATE eState)
{
	switch (eState)
	{
	case CMonsterN3::N3S_SPAWN:
		Change_State(CMonsterN3::N3S_FLY);
		break;

	case CMonsterN3::N3S_PREPARE:
		Change_State(CMonsterN3::N3S_RUSH);
		break;

	case CMonsterN3::N3S_RUSH:
		Change_State(CMonsterN3::N3S_FLY);
		break;
	}
}

CN3_AI* CN3_AI::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	CN3_AI* pN3_AI = new CN3_AI(pGraphicDev);

	if (FAILED(pN3_AI->Ready_AI(fDetectRange, fInteractRange, iInitState)))
	{
		Safe_Release(pN3_AI);
		MSG_BOX("CN3_AI Create Failed");
		return nullptr;
	}

	return pN3_AI;
}

CComponent* CN3_AI::Clone()
{
	return new CN3_AI(*this);
}

void CN3_AI::Free()
{
}
