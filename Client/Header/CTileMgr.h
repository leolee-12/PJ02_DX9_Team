#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include <vector>

class CTile;

class CTileMgr : public CBase
{
	DECLARE_SINGLETON(CTileMgr)

private:
	explicit CTileMgr();
	virtual ~CTileMgr();

public:
	HRESULT	Initialize(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::MAPDATA& mapData);
	_int	Update(const _float& fTimeDelta);
	void	LateUpdate(const _float& fTimeDelta);
	void	Render();

public:
	_int	Get_MapWidth() const { return m_iMapWidth; }
	_int	Get_MapHeight() const { return m_iMapHeight; }
	CTile*	Get_Tile(_int iX, _int iZ);

	void	Reset_For_SceneChange();

private:
	std::vector<CTile*>	m_vecTiles;
	_int				m_iMapWidth;
	_int				m_iMapHeight;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_bool				m_bInitialized;

private:
	virtual void Free();
};
