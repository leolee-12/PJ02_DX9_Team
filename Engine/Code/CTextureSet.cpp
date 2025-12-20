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

HRESULT CTextureSet::Ready_Texture(TEXTUREID eID, const vector<TEXINFO>& vecTexInfo)
{
	_uint iTexNum = vecTexInfo.size();

	for (_uint i = 0; i < iTexNum; ++i)
	{
		if (FAILED(Add_Texture(eID, vecTexInfo[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTextureSet::Add_Texture(TEXTUREID eID, const TEXINFO& tTexInfo)
{
	auto iter = m_mapTexture.find(tTexInfo.strState);

	if (iter != m_mapTexture.end())
		return E_FAIL;

	// 맵 범위 밖을 접근하면 런타임 에러, 먼저 요소 추가해준 뒤 접근
	iter = m_mapTexture.emplace(tTexInfo.strState, vector<IDirect3DBaseTexture9*>()).first;

	iter->second.reserve(tTexInfo.iCnt);

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

		iter->second.push_back(pTexture);
	}

	return S_OK;
}

void CTextureSet::Set_Texture(wstring strState, const _uint& iIndex)
{
	auto iter = m_mapTexture.find(strState);

	if (iter == m_mapTexture.end())
		return;

	if (iter->second.size() <= iIndex)
		return;

	m_pGraphicDev->SetTexture(0, iter->second[iIndex]);
}

_float CTextureSet::Get_TextureEnd(wstring strState)
{
	auto iter = m_mapTexture.find(strState);

	if (iter == m_mapTexture.end())
		return 0.f;
	// Get_TextureEnd = 벡터의 크기 = 마지막 이미지 번호 + 1 : 텍스처 탐색 미싱이 날 수 있으므로 작은 수를 빼준다
	return iter->second.size() - 0.001f;
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
