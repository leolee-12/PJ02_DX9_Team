#pragma once
#include "CUi.h"
#include "InventoryEnum.h"
#include "CItem.h"

class CInvenBack;
class CInvenSlot;
class CInvenItem;
class CInvenBtn;
class CInvenItemInfo;
class CInvenStarImg;
class CInvenRedFog;
class CFontUIOrtho;
class CInvenPlayer;
class CInvenPlayerHp;
class CInvenEquipItem;

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
	_int	Get_ItemCount(CItem::ITEMID _eid);
	void	Set_MessageChannel(IMessageChannel* pMessageChannel);

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
	// 기본
	CInvenBack*			m_pInvenBack;
	CInvenBtn*			m_pItemInvenBtn;
	CInvenBtn*			m_pPlayerInfoBtn;
	CInvenItemInfo*		m_pInvenItemInfo;

	// 플레이어 정보
	CInvenRedFog*		m_pInfoRedFog;
	CInvenStarImg*		m_pInfoStarImg;
	CFontUIOrtho*		m_pPlayerNameFont;
	CInvenPlayer*		m_pInvenPlyer;
	CInvenPlayerHp*		m_pInvenPlayerHp;
	vector<CInvenSlot*> m_vPlayerSlot;
	vector<CInvenEquipItem*> m_vPlayerEquipItem;
	// 소지품
	vector<CInvenSlot*> m_vSlot;
	vector<CInvenItem*> m_vItem;
	CFontUIOrtho* m_pResourceFont;

	INVENSTATE m_eCurInvenState;
	INVENSTATE m_ePreInvenState;
	INVENTORY_RENDER_TYPE m_eInvenRenderState;

	vector<_vec3>		m_vSlotLocalPos;
	// 인벤토리 
	_int	m_iCurItemCount;

	_vec3	m_vLerpStart;
	_vec3	m_vLerpEnd;
	_float	m_fLerpTime;

	_bool	m_bActive;
	_bool	m_bMsgRegistered;
};
