#pragma once
#include "CGameObject.h"

class CFontUI : public CGameObject
{
private:
	explicit CFontUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFontUI(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);
	virtual ~CFontUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	void			Set_Font(const wstring& strFont) { m_strFont = strFont; }
	void			Set_Text(const wstring& strText) { m_strText = strText; }
	void			Set_OwnerName(const wstring& strOwnerName) { m_strOwnerName = strOwnerName; }
	void			Set_RenderOwnerName(const wstring& strRenderOwnerName) { m_strRenderOwnerName = strRenderOwnerName; }
	void			Set_FontColor(const D3DXCOLOR& tColor) { m_tFontColor = tColor; }
	void			Set_Flags(DWORD dwFlags) { m_dwFlags = dwFlags; }

	void			Set_WorldPos(const _vec3& vPos) { m_vWorldPos = vPos; }
	void			Set_Scale(const _vec2& vScale) { m_vScale = vScale; }

	void			Active() { m_bActive = true; }
	void			UnActive() { m_bActive = false; }


public:

	static		CFontUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);


private:
	void					Update_CutText(const _float& fTimeDelta);
	void					Ready_Event();
	virtual		void		Free();

private:
	_bool	m_bActive;

	RECT	m_tRenderRect;

	wstring m_strFont;
	wstring m_strText;
	wstring m_strRenderOwnerName;

	wstring m_strOwnerName;

	D3DXCOLOR m_tFontColor;
	DWORD	m_dwFlags;

	_vec3  m_vWorldPos;
	_vec2  m_vScale;

	_uint  m_iCutText;
	_float m_fCutDelay;
	_bool  m_bEnd;
};


