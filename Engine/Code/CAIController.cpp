#include "CAIController.h"

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

void CAIController::Free()
{
	CComponent::Free();
}