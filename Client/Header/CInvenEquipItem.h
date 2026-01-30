#pragma once
#include "CUi.h"
#include "InventoryEnum.h"
#include "CItem.h"
#include "CItemDataTable.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

struct EquipItemData
{
	EquipmentItemID ID = EID_NONE;
	wstring Name = L"무기 장비되지않음";
	wstring Flavor = L"";
	wstring Description = L"재 무기를 장비하지 않았습니다\n무기를 찾아 장비하세요.";
	_bool	bOnHoverEvet = false;
};

class CFontUIOrtho;

class CInvenEquipItem :
	public CUi
{
private:
	explicit CInvenEquipItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInvenEquipItem();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CInvenEquipItem* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale);

public:
	void Set_Render(_bool _bRender) { m_bRender = _bRender; };
	void Set_ParentPos(_vec3 _vParentPos) { m_vParentPos = _vParentPos; }
	void Set_LocalPos(_vec3 _vLocalPos) { m_vLocalPos = _vLocalPos; }
	_bool Get_Redner() { return m_bRender; }
	const EquipItemData& Get_ItemData() const { return m_tItemData; }
	void Set_ItemData(const EquipItemData& _ItemData) { m_tItemData = _ItemData; }
	void   Set_MessageChannel(IMessageChannel* pMessageChannel);
	void Set_ItemID(EquipmentItemID _id) { m_tItemData.ID = _id; }
	void Set_HoverEvent(bool _bEvent) { m_tItemData.bOnHoverEvet = _bEvent; }
private:
	virtual			void		Free();
	void						OnHover_CusorColl();
	HRESULT						Add_Component();
	void						Ready_Event();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec2			m_vScreenPos;
	_vec2			m_vHitHalfScale;

	_vec3			m_vParentPos;
	_vec3			m_vLocalPos;
	_vec3			m_vWorldPos;

	_float m_fScale;
	_bool m_bRender;
	_int  m_iPage;

	_bool m_bHover;
	_bool m_bMsgRegistered;
private:
	EquipItemData m_tItemData;
};

