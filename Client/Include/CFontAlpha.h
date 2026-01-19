#pragma once
#include "CGameObject.h"

class CFontAlpha : public CGameObject
{
private:
	explicit CFontAlpha(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFontAlpha(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);
	virtual ~CFontAlpha();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	void			Set_Font(const wstring& strFont) { m_strFont = strFont; }
	void			Set_Text(const wstring& strText) { m_strText = strText; }
	void			Set_FontColor(const D3DXCOLOR& tColor) { m_tFontColor = tColor; }

	void			Active()
	{
		m_bActive = true;
	}
	void			UnActive()
	{
		m_bActive = false;
		m_bRevers = false;
	}


public:

	static		CFontAlpha* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);


private:
	void					Ready_Event();
	void					Update_Alpha();
	virtual		void		Free();

private:
	_bool	m_bActive;

	wstring m_strFont;
	wstring m_strText;

	D3DXCOLOR m_tFontColor = { 1.f, 1.f, 1.f, 0.f };

	_bool  m_bRevers = false;
};


