#pragma once

#include "CVIBuffer.h"
#include <optional>

BEGIN(Engine)

class ENGINE_DLL CTerrainWallTex : public CVIBuffer
{
protected:
	explicit CTerrainWallTex();
	explicit CTerrainWallTex(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CTerrainWallTex(const CTerrainWallTex& rhs);
	virtual ~CTerrainWallTex();

public:
	const _vec3* Get_VtxPos() { return m_pPos; }

public:
	HRESULT		Ready_Buffer(const _ulong& dwCntX,
		const _ulong& dwCntY,
		const _ulong& dwVtxItv,
		optional<wstring> Heightmappath);

	virtual		void		Render_Buffer();

private:
	HRESULT		Ready_Heightmap(const _ulong& dwCntX, const _ulong& dwCntY, const _ulong& dwVtxItv, wstring Heightmappath);
	HRESULT     Ready_Flat(const _ulong& dwCntX, const _ulong& dwCntY, const _ulong& dwVtxItv);

private:
	_vec3* m_pPos;

public:
	static CTerrainWallTex* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _ulong& dwCntX = VTXCNTX,
		const _ulong& dwCntY = VTXCNTX,
		const _ulong& dwVtxItx = VTXITV,
		optional<wstring> HeightMappath = nullopt); // optional < 값이 있을수도 있고 없을 수도 있는 변수



	virtual CComponent* Clone();
private:
	virtual		void	Free();
};

END
