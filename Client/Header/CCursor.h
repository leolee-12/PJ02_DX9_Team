#pragma once
#include "CUi.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CCursor :
	public CUi
{
private:
	explicit CCursor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCursor();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual	_int		Update_GameObject(const _float& fTimeDelta);
	virtual	void		Render_GameObject();

private:
	HRESULT				Add_Component();

	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CCursor* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();
};
