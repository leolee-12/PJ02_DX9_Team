#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CInvenSlot :
	public CUi
{
private:
	explicit CInvenSlot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInvenSlot();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);


public:
	static CInvenSlot* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale);

public:
	void Set_ParentPos(_vec3 _vParentPos) { m_vParentPos = _vParentPos; }
private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec3			m_vParentPos;
	_vec3			m_vLocalPos;
	_vec3			m_vWorldPos;

	_vec3 m_vPos;
	_float m_fScale;
	_bool m_bRender;
};

