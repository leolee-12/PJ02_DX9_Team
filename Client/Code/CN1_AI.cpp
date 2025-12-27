#include "pch.h"
#include "CN1_AI.h"

CN1_AI::CN1_AI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CAIController(pGraphicDev)
{
}

CN1_AI::CN1_AI(const CN1_AI& rhs)
	: CAIController(rhs)
{
}

CN1_AI::~CN1_AI()
{
}

HRESULT CN1_AI::Ready_AI(CGameObject* pOwner, const _float& fDetectRange, const _float& fInteractRange)
{
	if (FAILED(CAIController::Ready_AI(pOwner, fDetectRange, fInteractRange)))
		return E_FAIL;

	return S_OK;
}

void CN1_AI::Change_State()
{
}

void CN1_AI::Enter_State()
{
}

void CN1_AI::Exit_State()
{
}

CN1_AI* CN1_AI::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* pOwner, const _float& fDetectRange, const _float& fInteractRange)
{
	CN1_AI* pN1_AI = new CN1_AI(pGraphicDev);

	if (FAILED(Ready_AI(pOwner, fDetectRange, fInteractRange)))
	{
		Safe_Release(pN1_AI);
		MSG_BOX("CN1_AI Create Failed");
		return nullptr;
	}

	return pN1_AI;
}

CComponent* CN1_AI::Clone()
{
	return nullptr;
}

void CN1_AI::Free()
{
}