#include "pch.h"
#include "CN2_AI.h"
#include "CTransform.h"

CN2_AI::CN2_AI(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CAIController(pGraphicDev),
		m_fSpeed(0.f),
		m_fAcmlTime(0.f),
		m_bChase(false)
{
}

CN2_AI::CN2_AI(const CN2_AI& rhs)
	:	CAIController(rhs),
		m_fSpeed(rhs.m_fSpeed),
		m_fAcmlTime(rhs.m_fAcmlTime),
		m_bChase(false)
{
}

CN2_AI::~CN2_AI()
{
}

HRESULT CN2_AI::Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	if (FAILED(CAIController::Ready_AI(fDetectRange, fInteractRange, iInitState)))
		return E_FAIL;

	m_fSpeed = 0.5f;
	m_fAcmlTime = 0.f;
	m_iRcmState = _uint(CMonsterN2::N2S_CRAWL);

	return S_OK;
}

void CN2_AI::Enter_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterN2::N2S_CRAWL:
		m_fAcmlTime = 0.f;
		break;
	case CMonsterN2::N2S_JUMP:
	{
		m_fSpeed = 1.f;
	}
		break;
	case CMonsterN2::N2S_LAND:
	{
		m_fSpeed = 1.f;
	}
		break;
	case CMonsterN2::N2S_SPAWN:
		m_bActiveAI = false;
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

		_vec3 vDir;

		if (!m_bChase)	Randomize_Dir();
		else			Compute_TargetDir();
	}
	break;

	case CMonsterN2::N2S_JUMP:
	{
	}
	break;

	case CMonsterN2::N2S_LAND:
	{
		if (!m_pTargetTC) m_bChase = false;

		_vec3 vDir;

		Randomize_Dir();
	}
		break;

	case CMonsterN2::N2S_SPAWN:
		m_bActiveAI = true;
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
	}

	return iExit;
}

void CN2_AI::Update_Crawl(const _float& fTimeDelta)
{

}

void CN2_AI::Update_Jump(const _float& fTimeDelta)
{

}

void CN2_AI::Update_Land(const _float& fTimeDelta)
{

}

void CN2_AI::Update_Spawn(const _float& fTimeDelta)
{
}

void CN2_AI::Compute_Distance()
{
	if (!m_pOwnerTC || !m_pTargetTC)
	{
		m_fDistance = FLT_MAX;
		return;
	}

	_vec3 vOwnerPos, vTargetPos, vDir;
	m_pOwnerTC->Get_Info(INFO_POS, &vOwnerPos);
	m_pTargetTC->Get_Info(INFO_POS, &vTargetPos);
	vDir = vTargetPos - vOwnerPos;

	m_fDistance = D3DXVec3Length(&vDir);
}

void CN2_AI::Compute_TargetDir()
{
	if (!m_pOwnerTC || !m_pTargetTC) return;

	_vec3 vOwnerPos, vTargetPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vOwnerPos);
	m_pTargetTC->Get_Info(INFO_POS, &vTargetPos);
	m_vDir = vTargetPos - vOwnerPos;
	D3DXVec3Normalize(&m_vDir, &m_vDir);
}

void CN2_AI::Randomize_Dir()
{
	_float fAngle = D3DXToRadian(rand() % 360);
	m_vDir.x = cosf(fAngle);
	m_vDir.y = 0.f;
	m_vDir.z = sinf(fAngle);
}

void CN2_AI::Anim_End(CMonsterN2::MONSTER_N2_STATE eState)
{
	switch (eState)
	{
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