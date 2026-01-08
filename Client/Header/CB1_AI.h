#pragma once
#include "CAIController.h"
#include "CMonsterB1.h"

class CB1_AI : public CAIController
{
protected:
	explicit	CB1_AI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CB1_AI(const CB1_AI& rhs);
	virtual		~CB1_AI();

protected:
	HRESULT		Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState);
	void		Enter_State(const _uint& iState)	override;
	void		Exit_State(const _uint& iState)		override;

public:
	void		Set_Speed(const _float& fSpeed) { m_fSpeed = fSpeed; }
	void		Anim_End(CMonsterB1::MONSTER_B1_STATE eState);

private:
	_int		Update_Component(const _float& fTimeDelta)	override;
	void		Update_Crawl(const _float& fTimeDelta);
	void		Update_Jump(const _float& fTimeDelta);
	void		Update_Land(const _float& fTimeDelta);
	void		Update_Spawn(const _float& fTimeDelta);
	void		Update_Stop(const _float& fTimeDelta);

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
	static CB1_AI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState = 0);
	CComponent* Clone()	override;

protected:
	void				Free()	override;
};