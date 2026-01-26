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
	static CInvenSlot* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fScale);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec3 m_vPos;
	_float m_fScale;
	_bool m_bRender;
};

