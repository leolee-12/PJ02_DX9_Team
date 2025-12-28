#pragma once
#include "CComponent.h"

BEGIN(Engine)

class CGameObject;
class CTransform;

class ENGINE_DLL CAIController : public CComponent
{
protected:
	explicit	CAIController(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CAIController(const CAIController& rhs);
	virtual		~CAIController();

public:
	void		Set_ActiveAI(_bool bActive)						{ m_bActiveAI = bActive; }
	void		Set_OwnerTransform(CTransform* pTransformCom)	{ m_pOwnerTC = pTransformCom; }
	void		Set_TargetTransform(CTransform* pTransformCom)	{ m_pTargetTC = pTransformCom; }
	void		Set_DetectRange(_float fRange)					{ m_fDetectRange = fRange; }
	void		Set_InteractRange(_float fRange)				{ m_fInteractRange = fRange; }
	template <typename T>
	void		Set_State(T eState) { m_iCurState = _uint(eState); }

	_float		Get_DetectRange() const { return m_fDetectRange; }

	template <typename T>
	T			Get_RecommendState() const { return static_cast<T>(m_iRcmState); }

protected:
	virtual HRESULT		Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState)	PURE;
	virtual void		Enter_State(const _uint& pState)	PURE;
	virtual void		Exit_State(const _uint& pState)		PURE;
	void				Change_State(const _uint& pState);

protected:
	// AI 활성화 여부
	_bool			m_bActiveAI;

	// 상태 관련
	_uint			m_iPreState;	// 이전 상태
	_uint			m_iCurState;	// 현재 AI 상태
	_uint			m_iRcmState;	// Owner에게 추천할 상태

	// 탐색 관련
	CTransform*		m_pOwnerTC;
	CTransform*		m_pTargetTC;
	_float			m_fDetectRange;		// 탐지 거리
	_float			m_fInteractRange;	// 상호작용을 시도하는 거리
	_float			m_fDistance;		// 현재 타겟과의 거리
	_vec3			m_vDir;				// 현재 타겟으로의 방향
	

protected:
	virtual void Free();
};

END