#pragma once
#include "CItem.h"

class CActiveItem : public CItem
{
protected:
	explicit	CActiveItem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CActiveItem(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CActiveItem(const CActiveItem& rhs);
	virtual		~CActiveItem();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		OnCollision(CGameObject* pObject);

protected:
	void				Update_Idle(const _float& fTimeDelta);
	void				Update_Chase(const _float& fTimeDelta);

public:
	static CActiveItem* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _float fThrowRange);

protected:
	virtual void Free();

	// 상수
	static constexpr _float AUTO_CHASE_RANGE = 10.f;
};
