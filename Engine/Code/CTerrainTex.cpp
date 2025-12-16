#include "CTerrainTex.h"

CTerrainTex::CTerrainTex()
	: m_pPos(nullptr)
{
}

CTerrainTex::CTerrainTex(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev), m_pPos(nullptr)
{
}

CTerrainTex::CTerrainTex(const CTerrainTex& rhs)
	: CVIBuffer(rhs),
	m_pPos(rhs.m_pPos)
{
}

CTerrainTex::~CTerrainTex()
{
}

HRESULT CTerrainTex::Ready_Buffer(const _ulong& dwCntX,
	const _ulong& dwCntZ,
	const _ulong& dwVtxItv,
	optional<wstring> Heightmappath)
{
	// optional < 값이 있을수도 있고 없을 수도 있는 변수
	// 해당 객체를 생성할때 optional에 문자열이 들어가면(높이맵경로)
	// 높이맵을 적용시킨 지형을 만들고
	// 아니라면 평면지형 생성으로 수정
	// 참조) CTerrainTex::Create()

	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = dwCntX * dwCntZ;
	m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	m_pPos = new _vec3[m_dwVtxCnt];


	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	if (Heightmappath.has_value()) {
		// 높이맵 경로 지정 함 -> 높이맵 사용함
		if (FAILED(Ready_Heightmap(dwCntX, dwCntZ, dwVtxItv, Heightmappath.value()))) { return E_FAIL; }
	}

	if (!Heightmappath.has_value()) {
		// 높이맵 경로 지정 안함 -> 높이맵 사용안함
		if (FAILED(Ready_Flat(dwCntX, dwCntZ, dwVtxItv))) { return E_FAIL; }
	}

	return S_OK;
}

void CTerrainTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

HRESULT CTerrainTex::Ready_Heightmap(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv, wstring Heightmappath)
{
	/*m_hFile = CreateFile(Heightmappath.c_str(),
		GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == m_hFile)
		return E_FAIL;

	_ulong dwByte(0);

	ReadFile(m_hFile, &m_fH, sizeof(BITMAPFILEHEADER), &dwByte, NULL);
	ReadFile(m_hFile, &m_iH, sizeof(BITMAPINFOHEADER), &dwByte, NULL);

	_ulong* pPixel = new _ulong[m_iH.biWidth * m_iH.biHeight];

	ReadFile(m_hFile, pPixel, sizeof(_ulong) * m_iH.biWidth * m_iH.biHeight, &dwByte, NULL);*/

	LPDIRECT3DTEXTURE9 pHeightTex = nullptr;

	// 높이맵 포맷 범용성을 위해 다이렉트 제공 유틸함수로 대체 
	// 버텍스 사이즈랑 높이맵 사이즈가 맞지않아도 버텍스 사이즈에 맞게 높이맵 조정
	// 포맷은 D3DFMT_A8R8G8B8 픽셀당 32비트 포맷으로 조정
	// D3DPOOL_SYSTEMMEM 해당 이미지데이터는 시스템 메모리에 올려서 cpu만 직접 접근하게 관리 (픽셀 데이터 읽는 과정은 우리가 직접 코드로 cpu에게 연산시키기 때문)
	D3DXCreateTextureFromFileEx(m_pGraphicDev, Heightmappath.c_str(), dwCntX, dwCntZ,
		D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pHeightTex);

	D3DLOCKED_RECT lockedRect;
	pHeightTex->LockRect(0, &lockedRect, NULL, D3DLOCK_READONLY);

	BYTE* pBits = (BYTE*)lockedRect.pBits;
	_uint iPitch = lockedRect.Pitch;

	_uint iPixelByte = 4;

	_ulong* pPixel = new _ulong[dwCntX * dwCntZ];

	// 다이렉트 제공 유틸함수로 이미지를 읽어오면 원본 포맷과 관련 없이 새로 포맷을 생성하기 때문에
	// bmp의 시작픽셀이(왼쪽아래) 다이렉트 좌표계 기준으로 자동 조정됨(왼쪽 위)
	// 우리는 맵의 버텍스를 왼쪽아래부터 찍기 시작해서 이미지 데이터를 그대로 읽어오면 상하반전이 일어남
	// 그걸 수정해주는 코드 - 이후는 수업코드랑 동일
	for (int z = 0; z < VTXCNTZ; ++z) {
		BYTE* pRow = pBits + (VTXCNTZ - 1 - z) * iPitch;
		//BYTE* pRow = pBits + z * iPitch;
		for (int x = 0; x < VTXCNTX; ++x) {
			BYTE* pPixelperByte = pRow + x * iPixelByte;
			pPixel[z * VTXCNTZ + x] = *pPixelperByte;
		}
	}

	pHeightTex->UnlockRect(0);
	pHeightTex->Release();

	VTXTEX* pVertex = NULL;

	// &pVertex : 버텍스 버퍼에 저장된 정점 중 첫 번째 주소를 얻어 옴.

	_ulong	dwIndex(0);

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	for (_ulong i = 0; i < dwCntZ; ++i)
	{
		for (_ulong j = 0; j < dwCntX; ++j)
		{
			dwIndex = i * dwCntX + j;

			pVertex[dwIndex].vPosition =
			{
			  _float(j * dwVtxItv),
			  _float(pPixel[dwIndex] & 0x000000ff) / 20.f,
			  _float(i * dwVtxItv)
			};
			pVertex[dwIndex].vTexUV = { ((_float)j / (dwCntX - 1)) * 20.f ,
										((_float)i / (dwCntZ - 1)) * 20.f };

			m_pPos[dwIndex] = pVertex[dwIndex].vPosition;

			pVertex[dwIndex].vNormal = { 0.f, 0.f, 0.f };
		}
	}

	Safe_Delete_Array(pPixel);

	_vec3	vNormal, vDst, vSrc;

	INDEX32* pIndex = nullptr;

	_ulong	dwTriCnt(0);

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_ulong i = 0; i < dwCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1; ++j)
		{
			dwIndex = i * dwCntX + j;

			// 오른쪽 위
			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriCnt]._2 = dwIndex + 1;

			vDst = pVertex[pIndex[dwTriCnt]._1].vPosition - pVertex[pIndex[dwTriCnt]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriCnt]._2].vPosition - pVertex[pIndex[dwTriCnt]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDst, &vSrc);

			pVertex[pIndex[dwTriCnt]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._2].vNormal += vNormal;

			dwTriCnt++;

			// 왼쪽 아래
			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + 1;
			pIndex[dwTriCnt]._2 = dwIndex;

			vDst = pVertex[pIndex[dwTriCnt]._1].vPosition - pVertex[pIndex[dwTriCnt]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriCnt]._2].vPosition - pVertex[pIndex[dwTriCnt]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDst, &vSrc);

			pVertex[pIndex[dwTriCnt]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._2].vNormal += vNormal;

			dwTriCnt++;
		}
	}

	for (_uint i = 0; i < m_dwVtxCnt; ++i)
	{
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	return S_OK;
}

HRESULT CTerrainTex::Ready_Flat(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
{
	VTXTEX* pVertex = NULL;

	// &pVertex : 버텍스 버퍼에 저장된 정점 중 첫 번째 주소를 얻어 옴.

	_ulong	dwIndex(0);

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	for (_ulong i = 0; i < dwCntZ; ++i)
	{
		for (_ulong j = 0; j < dwCntX; ++j)
		{
			dwIndex = i * dwCntX + j;

			pVertex[dwIndex].vPosition =
			{
			  _float(j * dwVtxItv),
			  0.f,
			  _float(i * dwVtxItv)
			};
			pVertex[dwIndex].vTexUV = { ((_float)j / (dwCntX - 1)) * 20.f ,
										((_float)i / (dwCntZ - 1)) * 20.f };

			m_pPos[dwIndex] = pVertex[dwIndex].vPosition;

			pVertex[dwIndex].vNormal = { 0.f, 0.f, 0.f };
		}
	}

	_vec3	vNormal, vDst, vSrc;

	INDEX32* pIndex = nullptr;

	_ulong	dwTriCnt(0);

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_ulong i = 0; i < dwCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1; ++j)
		{
			dwIndex = i * dwCntX + j;

			// 오른쪽 위
			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriCnt]._2 = dwIndex + 1;

			vDst = pVertex[pIndex[dwTriCnt]._1].vPosition - pVertex[pIndex[dwTriCnt]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriCnt]._2].vPosition - pVertex[pIndex[dwTriCnt]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDst, &vSrc);

			pVertex[pIndex[dwTriCnt]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._2].vNormal += vNormal;

			dwTriCnt++;

			// 왼쪽 아래
			pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
			pIndex[dwTriCnt]._1 = dwIndex + 1;
			pIndex[dwTriCnt]._2 = dwIndex;

			vDst = pVertex[pIndex[dwTriCnt]._1].vPosition - pVertex[pIndex[dwTriCnt]._0].vPosition;
			vSrc = pVertex[pIndex[dwTriCnt]._2].vPosition - pVertex[pIndex[dwTriCnt]._1].vPosition;

			D3DXVec3Cross(&vNormal, &vDst, &vSrc);

			pVertex[pIndex[dwTriCnt]._0].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._1].vNormal += vNormal;
			pVertex[pIndex[dwTriCnt]._2].vNormal += vNormal;

			dwTriCnt++;
		}
	}

	for (_uint i = 0; i < m_dwVtxCnt; ++i)
	{
		D3DXVec3Normalize(&pVertex[i].vNormal, &pVertex[i].vNormal);
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	return S_OK;
}

CTerrainTex* CTerrainTex::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItx, optional<wstring> Heightmappath)
{
	CTerrainTex* pTerrainTex = new CTerrainTex(pGraphicDev);

	if (FAILED(pTerrainTex->Ready_Buffer(dwCntX, dwCntZ, dwVtxItx, Heightmappath)))
	{
		Safe_Release(pTerrainTex);
		MSG_BOX("pTerrainTex Create Failed");
		return nullptr;
	}

	return pTerrainTex;
}

CComponent* CTerrainTex::Clone()
{
	return new CTerrainTex(*this);
}

void CTerrainTex::Free()
{
	if (m_bClone == false)
		Safe_Delete_Array(m_pPos);

	CVIBuffer::Free();
}
