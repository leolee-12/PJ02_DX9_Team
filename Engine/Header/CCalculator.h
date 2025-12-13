#pragma once
#include "CComponent.h"

BEGIN(Engine)

class CTerrainTex;
class CTransform;

class ENGINE_DLL CCalculator :  public CComponent
{
private:
	explicit CCalculator(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCalculator(const CCalculator& rhs);
	virtual ~CCalculator();

public:
	HRESULT		Ready_Calculator();

	_float		Compute_HeightOnTerrain(const _vec3* pPos,
										const _vec3* pTerrainVtxPos,
										const _ulong& dwCntX,
										const _ulong& dwCntZ);

	_vec3		Picking_OnTerrain(HWND hWnd,
								  CTerrainTex* pTerrainBufferCom,
								  CTransform* pTerrainTransformCom);

	_bool		Check_Triangle(const _vec3& vPos, const _vec3& vDir, const _long lCellX, const _long lCellZ, const _vec3* vLocalVtx, _vec3* vOut);

public:
	static CCalculator* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();


};
END

