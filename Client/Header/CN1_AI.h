#pragma once
#include "CAIController.h"

class CN1_AI : public CAIController
{
protected:
	explicit	CN1_AI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CN1_AI(const CN1_AI& rhs);
	virtual		~CN1_AI();

protected:
	HRESULT		Ready_AI(CGameObject* pOwner, const _float& fDetectRange, const _float& fInteractRange);
	void		Change_State()	override;
	void		Enter_State()	override;
	void		Exit_State()	override;

public:
	static CN1_AI*		Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* pOwner, const _float& fDetectRange, const _float& fInteractRange);
	CComponent*			Clone()	override;

protected:
	void				Free()	override;
};