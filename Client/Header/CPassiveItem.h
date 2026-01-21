#pragma once
#include "CItem.h"

class CPassiveItem : public CItem
{
protected:
	explicit	CPassiveItem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CPassiveItem(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CPassiveItem(const CPassiveItem& rhs);
	virtual		~CPassiveItem();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		OnCollision(CGameObject* pObject);

protected:
	void				Update_Idle(const _float& fTimeDelta);
	void				Update_Summon(const _float& fTimeDelta);
	void				Interact();

protected:
	_bool	m_bTriggered;

public:
	static CPassiveItem* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _float fThrowRange);

protected:
	virtual void Free();
};
