#include "pch.h"
#include "CN3_AI.h"
#include "CTransform.h"

CN3_AI::CN3_AI(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CAIController(pGraphicDev),
		m_fSpeed(0.f),
		m_fAcmlTime(0.f),
		m_bChase(false)
{
}

CN3_AI::CN3_AI(const CN3_AI& rhs)
	:	CAIController(rhs),
		m_fSpeed(rhs.m_fSpeed),
		m_fAcmlTime(rhs.m_fAcmlTime),
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
	m_fAcmlTime = 0.f;
	m_iRcmState = _uint(CMonsterN3::N3S_FLY);

	return S_OK;
}

void CN3_AI::Enter_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterN3::N3S_SPAWN:
		m_bActiveAI = false;
		break;
	}
}

void CN3_AI::Exit_State(const _uint& iState)
{
	switch (iState)
	{
	case CMonsterN3::N3S_SPAWN:
		m_bActiveAI = true;
		break;
	}
}

_int CN3_AI::Update_Component(const _float& fTimeDelta)
{
	_int iExit(0);

	m_fAcmlTime += fTimeDelta;

	if (!m_bActiveAI) return iExit;

	Compute_Distance();

	switch (m_iCurState)
	{
	case CMonsterN3::N3S_SPAWN:
		Update_Spawn(fTimeDelta);
		break;
	}

	return iExit;
}

void CN3_AI::Update_Fly(const _float& fTimeDelta)
{
	if (!m_pTargetTC) return;

	m_pOwnerTC->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
}

void CN3_AI::Update_Prepare(const _float& fTimeDelta)
{
	if (m_fAcmlTime > 3.f) Compute_TargetDir();

}

void CN3_AI::Update_Rush(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vPos);

	D3DXVec3Lerp(&vPos, &vPos, &m_vLerpPos, m_fSpeed);

	m_pOwnerTC->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CN3_AI::Update_Spawn(const _float& fTimeDelta)
{
}

void CN3_AI::Compute_Distance()
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

void CN3_AI::Compute_TargetDir()
{
	if (!m_pOwnerTC || !m_pTargetTC) return;

	_vec3 vOwnerPos, vTargetPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vOwnerPos);
	m_pTargetTC->Get_Info(INFO_POS, &vTargetPos);
	m_vDir = vTargetPos - vOwnerPos;
	D3DXVec3Normalize(&m_vDir, &m_vDir);
}

void CN3_AI::Randomize_Dir()
{
	_float fAngle = D3DXToRadian(rand() % 360);
	m_vDir.x = cosf(fAngle);
	m_vDir.y = 0.f;
	m_vDir.z = sinf(fAngle);
}

void CN3_AI::Anim_End(CMonsterN3::MONSTER_N3_STATE eState)
{
	switch (eState)
	{
	case CMonsterN3::N3S_SPAWN:
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