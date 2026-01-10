#include "CRcTex.h"

CRcTex::CRcTex()
{
}

CRcTex::CRcTex(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVIBuffer(pGraphicDev)
{
}

CRcTex::CRcTex(const CRcTex& rhs)
	: CVIBuffer(rhs)
{
}

CRcTex::~CRcTex()
{
}

HRESULT CRcTex::Ready_Buffer(DWORD dwGridSize)
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = dwGridSize * dwGridSize;
	m_dwTriCnt = (dwGridSize - 1) * (dwGridSize - 1) * 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	const float fStep = 1.f / (dwGridSize - 1);

	for (DWORD row = 0; row < dwGridSize; ++row)
	{
		for (DWORD col = 0; col < dwGridSize; ++col)
		{
			DWORD idx = row * dwGridSize + col;

			float fX = -0.5f + col * fStep;
			float fY = 0.5f - row * fStep;

			pVertex[idx].vPosition = { fX, fY, 0.f };
			pVertex[idx].vTexUV = { col * fStep, row * fStep };
			pVertex[idx].vNormal = { 0.f, 0.f, -1.f };
		}
	}

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	DWORD triIdx = 0;
	for (DWORD row = 0; row < dwGridSize - 1; ++row)
	{
		for (DWORD col = 0; col < dwGridSize - 1; ++col)
		{
			DWORD topLeft = row * dwGridSize + col;
			DWORD topRight = topLeft + 1;
			DWORD bottomLeft = topLeft + dwGridSize;
			DWORD bottomRight = bottomLeft + 1;

			pIndex[triIdx]._0 = topLeft;
			pIndex[triIdx]._1 = topRight;
			pIndex[triIdx]._2 = bottomRight;
			++triIdx;

			pIndex[triIdx]._0 = topLeft;
			pIndex[triIdx]._1 = bottomRight;
			pIndex[triIdx]._2 = bottomLeft;
			++triIdx;
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

void CRcTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CRcTex* CRcTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, DWORD dwGridSize)
{
	CRcTex* pRcTex = new CRcTex(pGraphicDev);

	if (FAILED(pRcTex->Ready_Buffer(dwGridSize)))
	{
		Safe_Release(pRcTex);
		MSG_BOX("pRcTex Create Failed");
		return nullptr;
	}

	return pRcTex;
}

CComponent* CRcTex::Clone()
{
	return new CRcTex(*this);
}

void CRcTex::Free()
{
	CVIBuffer::Free();
}
