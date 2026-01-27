#pragma once
#include "CUi.h"
#include "InventoryEnum.h"
#include "CItem.h"
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
	void	Add_Item(CItem::ITEMID _eid, _int _iCount);
	BOOL	Use_Item(CItem::ITEMID _eid, _int _iCount);
	void	Set_State(INVENSTATE _estate) { m_eCurInvenState = _estate; }
	_int	GetItemCount(CItem::ITEMID _eid);
private:
	virtual void Free();
	void	SortLocalPos();
	void	Key_Input_Inven();
	void    Ready_Event();
	void Moveto(const _vec3& vStart, const _vec3& vEnd)
	{
		m_vLerpStart = vStart;
		m_vLerpEnd = vEnd;
		m_fLerpTime = 0.f;
	}

private:
	CInvenBack*		m_pInvenBack;
	CInvenBtn*		m_pItemInvenBtn;
	CInvenBtn*		m_pPlayerInfoBtn;

	vector<CInvenSlot*> m_vSlot;
	vector<CInvenItem*> m_vItem;
	vector<_vec3>		m_vSlotLocalPos;
	INVENSTATE m_eCurInvenState;
	INVENSTATE m_ePreInvenState;

	_int	m_iInvenCount;

	_vec3	m_vLerpStart;
	_vec3	m_vLerpEnd;
	_float	m_fLerpTime;

	_bool	m_bActive;

};
