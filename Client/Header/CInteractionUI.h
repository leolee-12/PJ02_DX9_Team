#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CInteractionUI :
	public CUi
{
private:
	explicit CInteractionUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInteractionUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

	void			Active() { m_bActive = true; }
	void			UnActive() { m_bActive = false; }

public:
	static CInteractionUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_bool	 m_bActive = false;
};

