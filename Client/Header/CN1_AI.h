#pragma once
#include "CAIController.h"

class CN1_AI : public CAIController
{
protected:
	explicit	CN1_AI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CN1_AI(const CN1_AI& rhs);
	virtual		~CN1_AI();

protected:
	HRESULT		Ready_AI(CGameObject* pOwner, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState);
	void		Change_State(const _uint& iState)	override;
	void		Enter_State(const _uint& iState)	override;
	void		Exit_State(const _uint& iState)		override;

private:
	_int		Update_Component(const _float& fTimeDelta)	override;
	_int		Update_Idle(const _float& fTimeDelta);
	_int		Update_Run(const _float& fTimeDelta);
	_int		Update_Attack(const _float& fTimeDelta);
	_int		Update_Hit(const _float& fTimeDelta);
	_int		Update_Spawn(const _float& fTimeDelta);
	_int		Update_Jeer(const _float& fTimeDelta);
	_int		Update_Pray(const _float& fTimeDelta);

	_float		Compute_Distance();

private:
	_float		m_fSpeed;
	_float		m_fAcmlTime;

public:
	static CN1_AI*		Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* pOwner, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState = 0);
	CComponent*			Clone()	override;

protected:
	void				Free()	override;
};