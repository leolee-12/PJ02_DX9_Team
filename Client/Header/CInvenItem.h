#pragma once
#include "CUi.h"
#include "InventoryEnum.h"
#include "CItem.h"
namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CFontUIOrtho;

class CInvenItem :
	public CUi
{
private:
	explicit CInvenItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInvenItem();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CInvenItem* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale);

public:
	void Set_Render(_bool _bRender) { m_bRender = _bRender; };
	void Set_ParentPos(_vec3 _vParentPos) { m_vParentPos = _vParentPos; }
	void Set_LocalPos(_vec3 _vLocalPos) { m_vLocalPos = _vLocalPos; }
	CItem::ITEMID Get_ItemID() { return m_eItemid; }
	void   Set_ItemID(CItem::ITEMID _eID) { m_eItemid = _eID; }
	_int Get_ItemCount() { return m_iCount; }
	void Set_ItemCount(_int _iCount) { m_iCount = _iCount; }
	_bool Get_Redner() { return m_bRender; }
private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	CFontUIOrtho* m_pCountFont;
	_vec3			m_vParentPos;
	_vec3			m_vLocalPos;
	_vec3			m_vWorldPos;

	_float m_fScale;
	_bool m_bRender;
	_int  m_iPage;

private:
	// 아이템 정보
	CItem::ITEMID	m_eItemid;
	_int	m_iCount;

};

