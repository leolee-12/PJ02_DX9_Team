#include "CAIController.h"
#include "CTransform.h"

CAIController::CAIController(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CComponent(pGraphicDev),
		m_iPreState(-1),
		m_iCurState(-1),
		m_iRcmState(-1),
		m_bActiveAI(false),
		m_pOwnerTC(nullptr),
		m_pTargetTC(nullptr),
		m_fDetectRange(0.f),
		m_fInteractRange(0.f),
		m_fDistance(0.f)
{
}

CAIController::CAIController(const CAIController& rhs)
	:	CComponent(rhs),
		m_bActiveAI(rhs.m_bActiveAI),
		m_iPreState(rhs.m_iPreState),
		m_iCurState(rhs.m_iCurState),
		m_iRcmState(rhs.m_iRcmState),
		m_pOwnerTC(nullptr),
		m_pTargetTC(nullptr),
		m_fDetectRange(rhs.m_fDetectRange),
		m_fInteractRange(rhs.m_fInteractRange),
		m_fDistance(rhs.m_fDistance),
		m_vDir(rhs.m_vDir)
{
}

CAIController::~CAIController()
{
}

HRESULT		CAIController::Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	m_fDetectRange = fDetectRange;
	m_fInteractRange = fInteractRange;
	m_iCurState = iInitState;
	m_vDir = {0.f, 0.f, 0.f};

	return S_OK;
}

void CAIController::Change_State(const _uint& iState)
{
	if (m_iCurState == iState) return;

	Exit_State(m_iCurState);

	m_iPreState = m_iCurState;
	m_iCurState = iState;

	Enter_State(m_iCurState);

	m_iRcmState = iState;
}

void CAIController::Compute_Distance()
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

_vec3 CAIController::Compute_TargetDir()
{
	if (!m_pOwnerTC || !m_pTargetTC) return _vec3{ 0.f, 0.f, 0.f };

	_vec3 vDir, vOwnerPos, vTargetPos;
	m_pOwnerTC->Get_Info(INFO_POS, &vOwnerPos);
	m_pTargetTC->Get_Info(INFO_POS, &vTargetPos);
	vDir = vTargetPos - vOwnerPos;
	D3DXVec3Normalize(&vDir, &vDir);

	return vDir;
}

_vec3 CAIController::Compute_LimitedDir(const _float& fMaxAngle, const _vec3& vCurDir, const _vec3& vDesiredDir)
{
	_vec3 v1, v2;
	D3DXVec3Normalize(&v1, &vCurDir);
	D3DXVec3Normalize(&v2, &vDesiredDir);

	_float fDot = max(-1.f, min(1.f, D3DXVec3Dot(&v1, &v2)));
	_float fRad = acosf(fDot);
	_float fMaxRad = D3DXToRadian(fMaxAngle);

	if (fRad <= fMaxRad)
		return v2;	// 최대 회전 각도보다 작으면 그대로 사용

	// 최대 회전 각도보다 크면 최대 회전 각도로 보정
	fRad = fMaxRad / fRad;

	_vec3 vResult;
	D3DXVec3Lerp(&vResult, &v1, &v2, fRad);
	D3DXVec3Normalize(&vResult, &vResult);
	return vResult;
}

_vec3 CAIController::Randomize_Dir()
{
	_vec3 vDir;

	_float fAngle = D3DXToRadian(rand() % 360);
	vDir.x = cosf(fAngle);
	vDir.y = 0.f;
	vDir.z = sinf(fAngle);

	return vDir;
}

void CAIController::Free()
{
	CComponent::Free();
}