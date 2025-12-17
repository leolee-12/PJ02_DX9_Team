#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CTestEffect : public CGameObject
{
private:
	explicit CTestEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTestEffect(const CGameObject& rhs);
	virtual ~CTestEffect();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();
private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_float	m_fFrame;

public:
	static CTestEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();

};

