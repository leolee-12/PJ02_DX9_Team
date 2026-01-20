#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include <vector>
#include <stack>

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

	// 씬 보관/복원용
	void	Push_State();
	void	Pop_State();

private:
	std::vector<CTile*>	m_vecTiles;
	_int				m_iMapWidth;
	_int				m_iMapHeight;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_bool				m_bInitialized;

	// 씬 보관용 스택
	struct TILE_STATE
	{
		std::vector<CTile*>	vecTiles;
		_int				iMapWidth;
		_int				iMapHeight;
		_bool				bInitialized;
	};
	std::stack<TILE_STATE>	m_stackState;

private:
	virtual void Free();

private:
	_bool m_bSkipFirstFrame = false;
};
