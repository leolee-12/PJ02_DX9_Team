#pragma once
#include "CGameObject.h"

class CFontUIOrtho : public CGameObject
{
private:
	explicit CFontUIOrtho(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFontUIOrtho(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);
	virtual ~CFontUIOrtho();

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
	void			Set_Flags(DWORD dwFlags) { m_dwFlags = dwFlags; }

	void			Set_Pos(const _vec2& vPos) { m_vPos = vPos; }
	void			Set_Scale(const _vec2& vScale) { m_vScale = vScale; }

	void			Active()
	{
		m_bActive = true;
	}
	void			UnActive()
	{
		m_bActive = false;
	}


public:

	static		CFontUIOrtho* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);


private:
	void					Ready_Event();
	virtual		void		Free();

private:
	_bool	m_bActive;

	RECT	m_tRenderRect;

	wstring m_strFont;
	wstring m_strText;

	D3DXCOLOR m_tFontColor;
	DWORD	m_dwFlags;

	_vec2  m_vPos;
	_vec2  m_vScale;
};


