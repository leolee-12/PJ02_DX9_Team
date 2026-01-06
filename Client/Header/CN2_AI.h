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
	void		Update_Crawl(const _float& fTimeDelta);
	void		Update_Jump(const _float& fTimeDelta);
	void		Update_Land(const _float& fTimeDelta);
	void		Update_Spawn(const _float& fTimeDelta);
	void		Update_Stop(const _float& fTimeDelta);

	void		Compute_Distance();
	_vec3		Compute_TargetDir();
	_vec3		Compute_LimitedDir(const _float& fAngle, const _vec3& vCurDir, const _vec3& vDesiredDir);
	_vec3		Randomize_Dir();

private:
	_float		m_fSpeed;
	_vec3		m_vSpeed;
	_float		m_fAngle;
	_float		m_fGravity;
	_float		m_fGroundY;
	_float		m_fAcmlTime;
	_bool		m_bChase;
	_vec3		m_vLerpPos;		// Lerp¿ë À§Ä¡

public:
	static CN2_AI*		Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState = 0);
	CComponent*			Clone()	override;

protected:
	void				Free()	override;
};