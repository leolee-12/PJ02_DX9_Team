#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}


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
	static CInvenItem* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fScale);

	void Set_Render(_bool _bRender) { m_bRender = _bRender; };

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec3 m_vPos;
	_float m_fScale;
	_int	m_iCount;

	_bool m_bRender;
};

