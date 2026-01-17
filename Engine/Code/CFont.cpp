#include "CFont.h"

CFont::CFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev), m_pSprite(nullptr), m_pFont(nullptr)
{
	m_pGraphicDev->AddRef();
}

CFont::~CFont()
{
}

HRESULT CFont::Ready_Font(const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight, const DWORD& dwCharsetFlag)
{
	D3DXFONT_DESC			tFont_Desc;
	ZeroMemory(&tFont_Desc, sizeof(D3DXFONT_DESC));

	tFont_Desc.CharSet = dwCharsetFlag;
	tFont_Desc.CharSet = DEFAULT_CHARSET;
	tFont_Desc.Width = iWidth;
	tFont_Desc.Height = iHeight;
	tFont_Desc.Weight = iWeight;
	lstrcpy(tFont_Desc.FaceName, pFontType);
	//wstring strTmp = pFontType;
	//wcscpy_s(tFont_Desc.FaceName, LF_FACESIZE, pFontType);


	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &tFont_Desc, &m_pFont)))
	{
		MSG_BOX("Font Create Failed");
		return E_FAIL;
	}

	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite)))
	{
		MSG_BOX("Sprite Create Failed");
		return E_FAIL;
	}
	

	return S_OK;
}

void CFont::Render_Font(const _tchar* pString, const _vec2* pPos, D3DXCOLOR Color, DWORD Flag)
{
	RECT rc {(_long)pPos->x, (_long)pPos->y};

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &rc, Flag, Color);

	m_pSprite->End();
}

void CFont::Render_Font(const _tchar* pString, const RECT& Rect, D3DXCOLOR Color, DWORD Flag)
{
	RECT rc = Rect;
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &rc, Flag, Color);

	m_pSprite->End();
}

CFont* CFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight, const DWORD& dwCharsetFlag)
{
	CFont* pFont = new CFont(pGraphicDev);

	if (FAILED(pFont->Ready_Font(pFontType, iWidth, iHeight, iWeight, dwCharsetFlag)))
	{
		Safe_Release(pFont);
		MSG_BOX("Font Create Failed");
		return nullptr;
	}

	return pFont;
}

void CFont::Free()
{
}
