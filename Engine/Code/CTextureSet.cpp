#include "CTextureSet.h"

CTextureSet::CTextureSet()
{
}

CTextureSet::CTextureSet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CTextureSet::CTextureSet(const CTextureSet& rhs)
	: CComponent(rhs)
{
	_uint iMapSize = m_mapTexture.size();

	m_mapTexture = rhs.m_mapTexture;

	for (auto iter = m_mapTexture.begin();
		iter != m_mapTexture.end(); iter++)
	{
		_uint iSize = iter->second.size();

		for (size_t j = 0; j < iSize; ++j)
		{
			iter->second[j]->AddRef();
		}
	}

}

CTextureSet::~CTextureSet()
{
}

HRESULT CTextureSet::Ready_Texture(TEXTUREID eID, vector<TEXINFO>& vecTexInfo)
{

	_uint iTexNum = vecTexInfo.size();

	for (_uint i = 0; i < iTexNum; ++i)
	{
		if (FAILED(Add_Texture(eID, vecTexInfo[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTextureSet::Add_Texture(TEXTUREID eID, TEXINFO tTexInfo)
{
	if (m_mapTexture.find(tTexInfo.strState) != m_mapTexture.end())
		return E_FAIL;

	vector<IDirect3DBaseTexture9*> tempVec;

	tempVec.reserve(tTexInfo.iCnt);

	IDirect3DBaseTexture9* pTexture = nullptr;

	for (_uint i = 0; i < tTexInfo.iCnt; ++i)
	{
		TCHAR szFileName[128] = L"";

		wsprintf(szFileName, tTexInfo.pPath, i);

		switch (eID)
		{
		case TEX_NORMAL:

			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DTEXTURE9*)&pTexture)))
				return E_FAIL;

			break;

		case TEX_CUBE:

			if (FAILED(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DCUBETEXTURE9*)&pTexture)))
				return E_FAIL;

			break;
		}

		tempVec.push_back(pTexture);
	}

	m_mapTexture.emplace(tTexInfo.strState, tempVec);

	return S_OK;
}

void CTextureSet::Set_Texture(wstring strState, const _uint& iIndex)
{
	if (m_mapTexture.find(strState) == m_mapTexture.end())
		return;

	if (m_mapTexture[strState].size() <= iIndex)
		return;

	m_pGraphicDev->SetTexture(0, m_mapTexture[strState][iIndex]);
}

_uint CTextureSet::Get_TextureEnd(wstring strState)
{
	if (m_mapTexture.find(strState) == m_mapTexture.end())
		return 0;

	return m_mapTexture[strState].size();
}

CTextureSet* CTextureSet::Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eID, vector<TEXINFO>& vecTexInfo)
{
	CTextureSet* pTexture = new CTextureSet(pGraphicDev);

	if (FAILED(pTexture->Ready_Texture(eID, vecTexInfo)))
	{
		Safe_Release(pTexture);
		MSG_BOX("Texture Create Failed");
		return nullptr;
	}

	return pTexture;
}

CComponent* CTextureSet::Clone()
{
	return new CTextureSet(*this);
}

void CTextureSet::Free()
{
	CComponent::Free();

	for (auto iter = m_mapTexture.begin(); iter != m_mapTexture.end(); iter++)
	{
		for_each(iter->second.begin(), iter->second.end(), Safe_Release<IDirect3DBaseTexture9*>);
		iter->second.clear();
	}
	m_mapTexture.clear();
}
