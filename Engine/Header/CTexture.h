#pragma once

#include "CComponent.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CTexture :   public CComponent
{
private:
	explicit CTexture();
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture();

public:
	virtual HRESULT Ready_Texture(TEXTUREID eID, const _tchar* pPath, const _uint& iCnt);
	virtual HRESULT Ready_Texture_FromMemory(TEXTUREID eID, const vector<vector<BYTE>>& vecTexData, const _uint& iCnt);
	virtual HRESULT Ready_Texture_FromFolder(TEXTUREID eID, const _tchar* pFolderPath);
	void			Set_Texture(const _uint& iIndex = 0);
	void			Set_TextureStage(const _uint& Stage, const _uint& iIndex = 0);
	_uint			Get_TextureCount() const { return static_cast<_uint>(m_vecTexture.size()); }

	bool			Get_TextureSize(_uint* pWidth, _uint* pHeight, const _uint& iIndex = 0);

	IDirect3DBaseTexture9* Get_TextureHandle(_int iIndex);


private:
	vector<IDirect3DBaseTexture9*>		m_vecTexture;

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eID,
		const _tchar* pPath, const _uint& iCnt = 1);

	static CTexture* CreateFromMemory(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eID, const vector<vector<BYTE>>& vecTexData, const _uint& iCnt = 1);

	static CTexture* CreateFromFolder(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eID,
		const _tchar* pFolderPath);
	virtual CComponent* Clone();

private:
	virtual void Free();

};


END
