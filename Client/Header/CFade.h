#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CFade :
	public CUi
{
private:
	explicit CFade(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFade();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	void						Active() { m_bActive = true; }
	void						UnActive() { m_bActive = false; }
	void						Set_FadeIn()
								{
									m_bFadeIn = true;
									m_fAlpha = 1.f;
								}
	void						Set_FadeOut()
								{
									m_bFadeIn = false;
									m_fAlpha = 0.f;
								}

public:
	static CFade* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	void						Update_Alpha();
	HRESULT 					Ready_PixelShader();

	void						Ready_Event();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_bool m_bActive = false;
	_float m_fAlpha = 0.f;
	_bool m_bFadeIn = false;

	_float m_fFadeSpeed = 0.01f;


	LPDIRECT3DPIXELSHADER9 m_pPixelShader;
};

