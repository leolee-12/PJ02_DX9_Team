#include "CRcTexXZ.h"

CRcTexXZ::CRcTexXZ()
{
}

CRcTexXZ::CRcTexXZ(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVIBuffer(pGraphicDev)
{
}

CRcTexXZ::CRcTexXZ(const CRcTexXZ& rhs)
	: CVIBuffer(rhs)
{
}

CRcTexXZ::~CRcTexXZ()
{
}

HRESULT CRcTexXZ::Ready_Buffer()
{

	// 20x20 grid for better Point Light interpolation
	const _int GRID_SIZE = 100;
	const _int VERTEX_COUNT = (GRID_SIZE + 1) * (GRID_SIZE + 1);  // 21x21 = 441
	const _int TRI_COUNT = GRID_SIZE * GRID_SIZE * 2;  // 800

	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = VERTEX_COUNT;
	m_dwTriCnt = TRI_COUNT;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	_float fStep = 1.f / GRID_SIZE;  // 0.05 step

	for (_int z = 0; z <= GRID_SIZE; ++z)
	{
		for (_int x = 0; x <= GRID_SIZE; ++x)
		{
			_int idx = z * (GRID_SIZE + 1) + x;

			pVertex[idx].vPosition = {
				-0.5f + x * fStep,  // X: -0.5 to 0.5
				0.f,                 // Y: 0
				0.5f - z * fStep    // Z: 0.5 to -0.5
			};
			pVertex[idx].vTexUV = {
				x * fStep,          // U: 0 to 1
				z * fStep           // V: 0 to 1
			};
			pVertex[idx].vNormal = { 0.f, 1.f, 0.f };
		}
	}

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;
	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_int triIdx = 0;
	for (_int z = 0; z < GRID_SIZE; ++z)
	{
		for (_int x = 0; x < GRID_SIZE; ++x)
		{
			_int topLeft = z * (GRID_SIZE + 1) + x;
			_int topRight = topLeft + 1;
			_int bottomLeft = topLeft + (GRID_SIZE + 1);
			_int bottomRight = bottomLeft + 1;

			// First triangle
			pIndex[triIdx]._0 = topLeft;
			pIndex[triIdx]._1 = topRight;
			pIndex[triIdx]._2 = bottomRight;
			++triIdx;

			// Second triangle
			pIndex[triIdx]._0 = topLeft;
			pIndex[triIdx]._1 = bottomRight;
			pIndex[triIdx]._2 = bottomLeft;
			++triIdx;
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

void CRcTexXZ::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CRcTexXZ* CRcTexXZ::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRcTexXZ* pRcTexXZ = new CRcTexXZ(pGraphicDev);

	if (FAILED(pRcTexXZ->Ready_Buffer()))
	{
		Safe_Release(pRcTexXZ);
		MSG_BOX("pRcTexXZ Create Failed");
		return nullptr;
	}

	return pRcTexXZ;
}

CComponent* CRcTexXZ::Clone()
{
	return new CRcTexXZ(*this);
}

void CRcTexXZ::Free()
{
	CVIBuffer::Free();
}
