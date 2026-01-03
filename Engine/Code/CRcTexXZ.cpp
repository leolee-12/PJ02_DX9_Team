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
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// XZ plane vertices (Y = 0, lying flat on ground)
	// Looking from above (top-down view):
	//   Z+
	//   ^
	//   |
	// [0]---[1]
	//  |     |
	// [3]---[2] --> X+

	pVertex[0].vPosition = { -0.5f, 0.f,  0.5f };  // top-left (X-, Z+)
	pVertex[0].vTexUV = { 0.f, 0.f };
	pVertex[0].vNormal = { 0.f, 1.f, 0.f };

	pVertex[1].vPosition = { 0.5f, 0.f,  0.5f };   // top-right (X+, Z+)
	pVertex[1].vTexUV = { 1.f, 0.f };
	pVertex[1].vNormal = { 0.f, 1.f, 0.f };

	pVertex[2].vPosition = { 0.5f, 0.f, -0.5f };   // bottom-right (X+, Z-)
	pVertex[2].vTexUV = { 1.f, 1.f };
	pVertex[2].vNormal = { 0.f, 1.f, 0.f };

	pVertex[3].vPosition = { -0.5f, 0.f, -0.5f };  // bottom-left (X-, Z-)
	pVertex[3].vTexUV = { 0.f, 1.f };
	pVertex[3].vNormal = { 0.f, 1.f, 0.f };

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// First triangle (top-right)
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// Second triangle (bottom-left)
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

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
