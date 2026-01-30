#pragma once
#include "CUi.h"
#include "CInvenItem.h"
#include "CItemDataTable.h"
namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CFontUIOrtho;
class CItemInfoBg;
class CInvenDeco;
class CInvenSlot;
class CInvenItem;
class CInvenEquipItem;

class CInvenItemInfo :
	public CUi
{
private:
	explicit CInvenItemInfo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInvenItemInfo();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CInvenItemInfo* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	void				   Set_ItemDataInit(CItem::ITEMID _itemID);
	void				   Set_ItemDataInit(EquipmentItemID _itemID);
	void				   Set_Active(_bool _Active) { m_bActive = _Active; }
private:
	virtual			void		Free();
	void						Ready_Event();

private:
	CItemInfoBg*			m_pInfoBg;
	CInvenSlot*				m_pInfoSlot;
	CInvenItem*				m_pInfoItem;
	CInvenEquipItem*		m_pEquipItem;
	CInvenSlot*				m_pInfoDescBg;


	CFontUIOrtho* m_pItemNameFont;
	CFontUIOrtho* m_pItemFlavorFont;
	CFontUIOrtho* m_pItemDescFont;

	_bool			m_bActive;
	_vec3		m_vPos;
};

