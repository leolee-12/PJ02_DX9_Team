#pragma once
#include "CAIController.h"
#include "CMonsterN2.h"

class CN2_AI : public CAIController
{
protected:
	explicit	CN2_AI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CN2_AI(const CN2_AI& rhs);
	virtual		~CN2_AI();

protected:
	HRESULT		Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState);
	void		Enter_State(const _uint& iState)	override;
	void		Exit_State(const _uint& iState)		override;

public:
	void		Set_Speed(const _float& fSpeed) { m_fSpeed = fSpeed; }
	void		Anim_End(CMonsterN2::MONSTER_N2_STATE eState);

private:
	_int		Update_Component(const _float& fTimeDelta)	override;
	void		Update_Idle(const _float& fTimeDelta);
	void		Update_Run(const _float& fTimeDelta);
	void		Update_Attack(const _float& fTimeDelta);
	void		Update_Hit(const _float& fTimeDelta);
	void		Update_Spawn(const _float& fTimeDelta);
	void		Update_Jeer(const _float& fTimeDelta);
	void		Update_Pray(const _float& fTimeDelta);

	void		Compute_Distance();
	void		Compute_TargetDir();
	void		Randomize_Dir();

private:
	_float		m_fSpeed;
	_float		m_fAcmlTime;
	_bool		m_bChase;
	_vec3		m_vLerpPos;		// Lerp¿ë À§Ä¡

public:
	static CN2_AI*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState = 0);
	CComponent*			Clone()	override;

protected:
	void				Free()	override;
};