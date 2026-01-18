#pragma once
#include "CAIController.h"
#include "CFollower.h"

class CFollower_AI : public CAIController
{
protected:
	explicit	CFollower_AI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CFollower_AI(const CFollower_AI& rhs);
	virtual		~CFollower_AI();

protected:
	HRESULT		Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState);
	void		Enter_State(const _uint& iState)	override;
	void		Exit_State(const _uint& iState)		override;

public:
	void		Set_Speed(const _float& fSpeed) { m_fSpeed = fSpeed; }
	void		Anim_End(CFollower::FOLLOWER_STATE eState);
	_bool		Is_Chasing() { return m_bChase; }
	void		Set_LerpPos(const _vec3& vLerpPos) { m_vLerpPos = vLerpPos; }

private:
	_int		Update_Component(const _float& fTimeDelta)	override;
	void		Update_Idle(const _float& fTimeDelta);
	void		Update_Run(const _float& fTimeDelta);
	void		Update_Dance(const _float& fTimeDelta);
	void		Update_Transform(const _float& fTimeDelta);
	void		Update_Unconvert(const _float& fTimeDelta);
	void		Update_Convert(const _float& fTimeDelta);
	void		Update_Action(const _float& fTimeDelta);
	void		Update_Recruit(const _float& fTimeDelta);

private:
	_float		m_fSpeed;
	_float		m_fAcmlTime;
	_bool		m_bChase;
	_vec3		m_vLerpPos;		// Lerp용 위치
	_float		m_fWorkSpeed;

	static constexpr _float AUTO_ESCAPE_STATE_TIME = 3.f;
	static constexpr _float FW_DEFAULT_SPEED = 3.f;
	static constexpr _float FW_DEFAULT_WORK_SPEED = 1.f;

public:
	static CFollower_AI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState = 0);
	CComponent* Clone()	override;

protected:
	void				Free()	override;
};
