#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CTarotInfo :
	public CUi
{
private:
	explicit CTarotInfo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTarotInfo();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	void			Active() { m_bActive = true; }
	void			UnActive() { m_bActive = false; }

public:
	static CTarotInfo* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 _vPos, _uint iType);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_bool m_bActive = false;

	_vec2 m_vCenterPos;
	_uint m_iType;
};

