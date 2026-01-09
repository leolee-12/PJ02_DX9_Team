#include "CGrassBuffer.h"

CGrassBuffer::CGrassBuffer()
	: m_vOriginalNormal(0.f, 0.f, -1.f)
{
	ZeroMemory(m_vOriginalPos, sizeof(m_vOriginalPos));
	ZeroMemory(m_vOriginalUV, sizeof(m_vOriginalUV));
}

CGrassBuffer::CGrassBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
	, m_vOriginalNormal(0.f, 0.f, -1.f)
{
	ZeroMemory(m_vOriginalPos, sizeof(m_vOriginalPos));
	ZeroMemory(m_vOriginalUV, sizeof(m_vOriginalUV));
}

CGrassBuffer::CGrassBuffer(const CGrassBuffer& rhs)
	: CVIBuffer(rhs.m_pGraphicDev)  // Use device constructor, NOT copy constructor (to avoid buffer sharing)
	, m_vOriginalNormal(rhs.m_vOriginalNormal)
{
	// Copy member values from rhs
	m_dwVtxSize = rhs.m_dwVtxSize;
	m_dwVtxCnt = rhs.m_dwVtxCnt;
	m_dwTriCnt = rhs.m_dwTriCnt;
	m_dwFVF = rhs.m_dwFVF;
	m_dwIdxSize = rhs.m_dwIdxSize;
	m_IdxFmt = rhs.m_IdxFmt;
	m_bClone = true;

	// Copy original data
	for (_int i = 0; i < 4; ++i)
	{
		m_vOriginalPos[i] = rhs.m_vOriginalPos[i];
		m_vOriginalUV[i] = rhs.m_vOriginalUV[i];
	}

	// Create own dynamic vertex buffer (NOT shared!)
	if (FAILED(m_pGraphicDev->CreateVertexBuffer(
		m_dwVtxCnt * m_dwVtxSize,
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		m_dwFVF,
		D3DPOOL_DEFAULT,
		&m_pVB,
		NULL)))
	{
		MSG_BOX("CGrassBuffer Clone VB Create Failed");
		return;
	}

	// Initialize vertex data
	VTXTEX* pVertex = nullptr;
	if (SUCCEEDED(m_pVB->Lock(0, 0, (void**)&pVertex, D3DLOCK_DISCARD)))
	{
		for (_int i = 0; i < 4; ++i)
		{
			pVertex[i].vPosition = m_vOriginalPos[i];
			pVertex[i].vNormal = m_vOriginalNormal;
			pVertex[i].vTexUV = m_vOriginalUV[i];
		}
		m_pVB->Unlock();
	}

	// Share index buffer (it never changes)
	m_pIB = rhs.m_pIB;
	if (m_pIB)
		m_pIB->AddRef();
}

CGrassBuffer::~CGrassBuffer()
{
}

HRESULT CGrassBuffer::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_TEX;
	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	// Create DYNAMIC vertex buffer for per-frame updates
	if (FAILED(m_pGraphicDev->CreateVertexBuffer(
		m_dwVtxCnt * m_dwVtxSize,
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,  // Dynamic buffer for frequent updates
		m_dwFVF,
		D3DPOOL_DEFAULT,  // GPU memory (required for dynamic)
		&m_pVB,
		NULL)))
		return E_FAIL;

	// Create static index buffer (never changes)
	if (FAILED(m_pGraphicDev->CreateIndexBuffer(
		m_dwTriCnt * m_dwIdxSize,
		0,
		m_IdxFmt,
		D3DPOOL_MANAGED,
		&m_pIB,
		NULL)))
		return E_FAIL;

	// Initialize vertex data
	VTXTEX* pVertex = nullptr;
	if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertex, D3DLOCK_DISCARD)))
		return E_FAIL;

	// Top-left
	pVertex[0].vPosition = { -0.5f, 0.5f, 0.f };
	pVertex[0].vTexUV = { 0.f, 0.f };
	pVertex[0].vNormal = { 0.f, 0.f, -1.f };

	// Top-right
	pVertex[1].vPosition = { 0.5f, 0.5f, 0.f };
	pVertex[1].vTexUV = { 1.f, 0.f };
	pVertex[1].vNormal = { 0.f, 0.f, -1.f };

	// Bottom-right
	pVertex[2].vPosition = { 0.5f, -0.5f, 0.f };
	pVertex[2].vTexUV = { 1.f, 1.f };
	pVertex[2].vNormal = { 0.f, 0.f, -1.f };

	// Bottom-left
	pVertex[3].vPosition = { -0.5f, -0.5f, 0.f };
	pVertex[3].vTexUV = { 0.f, 1.f };
	pVertex[3].vNormal = { 0.f, 0.f, -1.f };

	// Store original positions for sway calculation
	for (_int i = 0; i < 4; ++i)
	{
		m_vOriginalPos[i] = pVertex[i].vPosition;
		m_vOriginalUV[i] = pVertex[i].vTexUV;
	}

	m_pVB->Unlock();

	// Initialize index buffer
	INDEX32* pIndex = nullptr;
	if (FAILED(m_pIB->Lock(0, 0, (void**)&pIndex, 0)))
		return E_FAIL;

	// Upper-right triangle
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// Lower-left triangle
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void CGrassBuffer::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

void CGrassBuffer::Set_TopVertexOffset(_float fOffsetX)
{
	if (nullptr == m_pVB)
		return;

	VTXTEX* pVertex = nullptr;

	// D3DLOCK_DISCARD: Discard previous data for better performance
	// GPU can continue using old buffer while we write to new one
	if (FAILED(m_pVB->Lock(0, 0, (void**)&pVertex, D3DLOCK_DISCARD)))
		return;

	// Top vertices (indices 0, 1) - apply X offset for sway
	pVertex[0].vPosition = m_vOriginalPos[0];
	pVertex[0].vPosition.x += fOffsetX;
	pVertex[0].vNormal = m_vOriginalNormal;
	pVertex[0].vTexUV = m_vOriginalUV[0];

	pVertex[1].vPosition = m_vOriginalPos[1];
	pVertex[1].vPosition.x += fOffsetX;
	pVertex[1].vNormal = m_vOriginalNormal;
	pVertex[1].vTexUV = m_vOriginalUV[1];

	// Bottom vertices (indices 2, 3) - keep fixed
	pVertex[2].vPosition = m_vOriginalPos[2];
	pVertex[2].vNormal = m_vOriginalNormal;
	pVertex[2].vTexUV = m_vOriginalUV[2];

	pVertex[3].vPosition = m_vOriginalPos[3];
	pVertex[3].vNormal = m_vOriginalNormal;
	pVertex[3].vTexUV = m_vOriginalUV[3];

	m_pVB->Unlock();
}

CGrassBuffer* CGrassBuffer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGrassBuffer* pInstance = new CGrassBuffer(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CGrassBuffer Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CGrassBuffer::Clone()
{
	return new CGrassBuffer(*this);
}

void CGrassBuffer::Free()
{
	// D3DPOOL_DEFAULT buffers must be released properly
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	// Don't call CVIBuffer::Free() as we already released buffers
	CComponent::Free();
}
