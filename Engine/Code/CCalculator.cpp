#include "CCalculator.h"

#include "CTerrainTex.h"
#include "CTransform.h"

CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev)
{
  

}

CCalculator::CCalculator(const CCalculator& rhs)
    : CComponent(rhs)
{
}

CCalculator::~CCalculator()
{
}

HRESULT CCalculator::Ready_Calculator()
{
    return S_OK;
}

_float CCalculator::Compute_HeightOnTerrain(const _vec3* pPos, const _vec3* pTerrainVtxPos, const _ulong& dwCntX, const _ulong& dwCntZ)
{
    _ulong  dwIndex = _ulong(pPos->z / 1.f) * dwCntX + _ulong(pPos->x / 1.f);

    if (dwIndex < 0 || dwIndex > (dwCntX - 1) * (dwCntZ - 1)) { return pPos->y; }

    _float  fRatioX = (pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / 1.f;
    _float  fRatioZ = (pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z ) / 1.f;

    D3DXPLANE   Plane;

    // 오른쪽 위
    if (fRatioX > fRatioZ)
    {
        D3DXPlaneFromPoints(&Plane, 
            &pTerrainVtxPos[dwIndex + dwCntX],
            &pTerrainVtxPos[dwIndex + dwCntX + 1], 
            &pTerrainVtxPos[dwIndex + 1]);

    }

    // 왼쪽 아래
    else
    {
        D3DXPlaneFromPoints(&Plane,
            &pTerrainVtxPos[dwIndex + dwCntX],
            &pTerrainVtxPos[dwIndex + 1],
            &pTerrainVtxPos[dwIndex]);
    }

  // ax + by + cz + d
  //    (- ax - cz - d) / b
  //
  //     y = (-ax - cz - d) / b
  //

    return ((-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b) + 1.f;
}

_vec3 CCalculator::Picking_OnTerrain(HWND hWnd, CTerrainTex* pTerrainBufferCom, CTransform* pTerrainTransformCom)
{
    POINT pt{};
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    _vec3 vMouse = _vec3((float)pt.x, (float)pt.y, 0.f);
    _vec3 vPos, vDir;

    _D3DVIEWPORT9 viewport;
    _matrix matProj, matView, matWorld;
    m_pGraphicDev->GetViewport(&viewport);
    m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    matWorld = *(pTerrainTransformCom->Get_World());
    D3DXVec3Unproject(
        &vPos,
        &vMouse,
        &viewport,
        &matProj,
        &matView,
        &matWorld
    );

    vMouse.z = 1.f;

    D3DXVec3Unproject(
        &vDir,
        &vMouse,
        &viewport,
        &matProj,
        &matView,
        &matWorld
    );

    vDir = vDir - vPos;
    D3DXVec3Normalize(&vDir, &vDir);

    // 아래에서 행하는 계산을 위해서는 방향 벡터가 "무조건" 정규화 되어 있어야 함

    _long lCellX = (_long)floor(vPos.x);
    _long lCellZ = (_long)floor(vPos.z);

    // 해당 축이 양의 방향인지 음의 방향인지
    _long lStepX = vDir.x > 0 ? 1 : -1;
    _long lStepZ = vDir.z > 0 ? 1 : -1;

    // 각 축의 셀을 하나 건너 뛸 때마다 필요한 거리
    // 예) 방향 벡터의 x 성분이 1 이면 1칸 이동 할 때 마다 1만큼 이동해야함
    // 반대로 0.5 이면 1칸 이동 할 때 마다 2만큼 이동해야함
    // 따라서 1 나누기 방향 벡터의 성분 절댓값이 필요거리
    // *필요거리 = 방향벡터의 성분 절댓값의 역수*
    _float fDeltaX = vDir.x == 0 ? FLT_MAX : fabsf(1.f / vDir.x);
    _float fDeltaZ = vDir.z == 0 ? FLT_MAX : fabsf(1.f / vDir.z);

    // 광선의 시작 위치에서 처음으로 셀 경계에 도달할 때까지의 거리
    // 예) 방향 벡터의 x 성분이 양수이고 시작 위치의 x 좌표가 2.3이면
    //     다음 셀 경계는 x = 3.0 이고, 남은 거리는 0.7임 (x축의 직선거리)
    //     하지만 광선은 대각으로 이동 할 수 있기 때문에
    //     이 거리를 방향 벡터의 x 성분 절댓값으로 나누어야함
    //     (대각선 거리 = 직선 거리 / cos세타, 여기서 cos세타는 방향 벡터의 x 성분 절댓값)
    _float fMaxX = vDir.x == 0 ? FLT_MAX : (lStepX > 0 ? (lCellX + 1.f - vPos.x) : (vPos.x - lCellX)) / fabsf(vDir.x);
    _float fMaxZ = vDir.z == 0 ? FLT_MAX : (lStepZ > 0 ? (lCellZ + 1.f - vPos.z) : (vPos.z - lCellZ)) / fabsf(vDir.z);

    _float fMaxDistance = 100.f; // 최대 탐색 거리
    _float fDistance = 0.f;		 // 현재까지의 탐색 거리 (이동 거리)

    _vec3  vOut;

    for (_ulong i = 0; i < VTXCNTX * VTXCNTZ; ++i) {
        if (lCellX >= 0 && lCellX < VTXCNTX &&
            lCellZ >= 0 && lCellZ < VTXCNTZ)
        {
            // 지형안에 광선이 존재할때 충돌체크
            if (Check_Triangle(vPos, vDir, lCellX, lCellZ, pTerrainBufferCom->Get_VtxPos(), &vOut)) {
                // 플레이어 좌표를 지형의 로컬좌표로 변환해서 계산을 했기 때문에
                // 지형의 월드행렬을 다시 곱해줘서 월드 스페이스 좌표로 변환 시켜서 리턴
                D3DXVec3TransformCoord(&vOut, &vOut, &matWorld);
                return vOut;
            }
        }

        if (fMaxX < fMaxZ) {
            // X축 진행
            fMaxX += fDeltaX;
            fDistance = fMaxX;
            lCellX += lStepX;
        }
        else {
            // Z축 진행
            fMaxZ += fDeltaZ;
            fDistance = fMaxZ;
            lCellZ += lStepZ;
        }

        if (fDistance > fMaxDistance) {
            break;
        }
    }

    return _vec3(0.f, 0.f, 0.f);
}

_bool CCalculator::Check_Triangle(const _vec3& vPos, const _vec3& vDir, const _long lCellX, const _long lCellZ, const _vec3* vLocalVtx, _vec3* vOut)
{
    _long iIndex0 = lCellZ * VTXCNTX + lCellX;
    _long iIndex1 = iIndex0 + 1;			
    _long iIndex2 = iIndex0 + VTXCNTX;		
    _long iIndex3 = iIndex2 + 1;			

    _vec3 v0, v1, v2, v3;

    v0 = vLocalVtx[iIndex0];
    v1 = vLocalVtx[iIndex1];
    v2 = vLocalVtx[iIndex2];
    v3 = vLocalVtx[iIndex3];


    _float fU, fV, fDist;

    if (D3DXIntersectTri(&v2, &v3, &v1, &vPos, &vDir, &fU, &fV, &fDist)) {
        *vOut = vPos + vDir * fDist;
        return true;
    }

    if (D3DXIntersectTri(&v2, &v1, &v0, &vPos, &vDir, &fU, &fV, &fDist)) {
        *vOut = vPos + vDir * fDist;
        return true;
    }

    return false;
}

CCalculator* CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCalculator* pCalculator = new CCalculator(pGraphicDev);

    if (FAILED(pCalculator->Ready_Calculator()))
    {
        Safe_Release(pCalculator);
        MSG_BOX("Calculator Create Failed");
        return nullptr;
    }

    return pCalculator;
}

CComponent* CCalculator::Clone()
{
    return new CCalculator(*this);
}

void CCalculator::Free()
{
    Safe_Release(m_pGraphicDev);
}
