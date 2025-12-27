#include "CAIController.h"

CAIController::CAIController(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CComponent(pGraphicDev),
		m_iPreState(-1),
		m_iCurState(-1),
		m_iRcmState(-1),
		m_bActiveAI(false),
		m_pOwner(nullptr),
		m_pTarget(nullptr),
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
		m_pOwner(nullptr),
		m_pTarget(nullptr),
		m_fDetectRange(rhs.m_fDetectRange),
		m_fInteractRange(rhs.m_fInteractRange),
		m_fDistance(rhs.m_fInteractRange)
{
}

CAIController::~CAIController()
{
}

HRESULT		CAIController::Ready_AI(CGameObject* pOwner, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)
{
	if(pOwner == nullptr)
		return E_FAIL;

	m_pOwner = pOwner;
	m_fDetectRange = fDetectRange;
	m_fInteractRange = fInteractRange;
	m_iCurState = iInitState;

	return S_OK;
}

void CAIController::Free()
{
	CComponent::Free();
}