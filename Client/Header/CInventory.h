#pragma once
#include "CUi.h"


class CInvenBack;
class CInvenSlot;
class CInvenItem;
class CInvenBtn;



class CInventory :
	public CUi
{
private:
	explicit CInventory(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInventory();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void    LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject();

public:
	static CInventory* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	void	Set_Active(_bool bActive) { m_bActive = bActive; }
private:
	virtual void Free();
	void	Key_Input_Inven();
	void    Ready_Event();


private:
	CInvenBack*		m_pInvenBack;
	CInvenBtn*		m_pInvenBtn;
	CInvenSlot*		m_pInvenSlot;
	CInvenItem*		m_pInvenItem;

	vector<CInvenSlot*> m_vSlot;

	_bool	m_bActive;

};
