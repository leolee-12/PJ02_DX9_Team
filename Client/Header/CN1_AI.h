#pragma once
#include "CAIController.h"
#include "CMonsterN1.h"

class CN1_AI : public CAIController
{
protected:
	explicit	CN1_AI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CN1_AI(const CN1_AI& rhs);
	virtual		~CN1_AI();

protected:
	HRESULT		Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState);
	void		Enter_State(const _uint& iState)	override;
	void		Exit_State(const _uint& iState)		override;

public:
	void		Set_Speed(const _float& fSpeed) { m_fSpeed = fSpeed; }
	void		Anim_End(CMonsterN1::MONSTER_N1_STATE eState);

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
	static CN1_AI*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState = 0);
	CComponent*			Clone()	override;

protected:
	void				Free()	override;
};