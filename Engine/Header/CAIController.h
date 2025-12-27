#pragma once
#include "CComponent.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CAIController : public CComponent
{
protected:
	explicit	CAIController(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CAIController(const CAIController& rhs);
	virtual		~CAIController();

public:
	void		Set_ActiveAI(_bool bActive)				{ m_bActiveAI = bActive; }
	void		Set_Target(CGameObject* pGameObject)	{ m_pTarget = pGameObject; }
	void		Set_DetectRange(_float fRange)			{ m_fDetectRange = fRange; }
	void		Set_InteractRange(_float fRange)		{ m_fInteractRange = fRange; }

	_float		Get_DetectRange() const { return m_fDetectRange; }

protected:
	virtual HRESULT		Ready_AI(CGameObject* pOwner, const _float& fDetectRange, const _float& fInteractRange)	PURE;
	virtual void		Change_State()	PURE;
	virtual void		Enter_State()	PURE;
	virtual void		Exit_State()	PURE;

protected:
	// AI 활성화 여부
	_bool			m_bActiveAI;

	// 탐색 관련
	CGameObject*	m_pOwner;
	CGameObject*	m_pTarget;
	_float			m_fDetectRange;		// 탐지 거리
	_float			m_fInteractRange;	// 상호작용을 시도하는 거리

protected:
	virtual void Free();
};

END