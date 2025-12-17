#pragma once

#include "CComponent.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CTextureSet : public CComponent
{
public:
	typedef struct tagTextureInfo
	{
		wstring			strState;
		const _tchar*	pPath;
		_uint			iCnt;

		tagTextureInfo() : strState(L""), pPath(nullptr), iCnt(0) {}
		tagTextureInfo(wstring _strState, const _tchar* _pPath, _uint _iCnt)
			: strState(_strState), pPath(_pPath), iCnt(_iCnt) {}
	}TEXINFO;

private:
	explicit CTextureSet();
	explicit CTextureSet(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTextureSet(const CTextureSet& rhs);
	virtual ~CTextureSet();

public:
	virtual HRESULT Ready_Texture(TEXTUREID eID, vector<TEXINFO>& vecTexInfo);
	HRESULT			Add_Texture(TEXTUREID eID, TEXINFO tTexInfo);
	void			Set_Texture(wstring strState, const _uint& iIndex = 0);

	_uint			Get_TextureEnd(wstring strState);

private:
	unordered_map<wstring, vector<IDirect3DBaseTexture9*>>		m_mapTexture;

public:
	static CTextureSet* Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eID, vector<TEXINFO>& vecTexInfo);
	virtual CComponent* Clone();

private:
	virtual void Free();

};


END
