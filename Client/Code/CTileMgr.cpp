#include "pch.h"
#include "CTileMgr.h"
#include "CTile.h"

IMPLEMENT_SINGLETON(CTileMgr)

CTileMgr::CTileMgr()
	: m_iMapWidth(0)
	, m_iMapHeight(0)
	, m_pGraphicDev(nullptr)
	, m_bInitialized(false)
{
}

CTileMgr::~CTileMgr()
{
	Free();
}

HRESULT CTileMgr::Initialize(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::MAPDATA& mapData)
{
	m_pGraphicDev = pGraphicDev;
	m_pGraphicDev->AddRef();

	m_iMapWidth = mapData.width;
	m_iMapHeight = mapData.height;

	// Create tiles from map data
	for (const auto& tileData : mapData.tiles)
	{
		CTile* pTile = CTile::Create(pGraphicDev, tileData);
		if (nullptr == pTile)
		{
			MSG_BOX("CTileMgr: Failed to create tile");
			continue;
		}
		m_vecTiles.push_back(pTile);
	}

	m_bInitialized = true;
	m_bSkipFirstFrame = true;

	return S_OK;
}

_int CTileMgr::Update(const _float& fTimeDelta)
{
	if (!m_bInitialized)
		return 0;

	if (m_bSkipFirstFrame)
	{
		m_bSkipFirstFrame = false;
		return 0;
	}

	for (auto& pTile : m_vecTiles)
	{
		if (pTile)
			pTile->Update_GameObject(fTimeDelta);
	}
	return 0;
}

void CTileMgr::LateUpdate(const _float& fTimeDelta)
{
	if (!m_bInitialized)
		return;

	for (auto& pTile : m_vecTiles)
	{
		if (pTile)
			pTile->LateUpdate_GameObject(fTimeDelta);
	}
}

void CTileMgr::Render()
{
	for (auto& pTile : m_vecTiles)
	{
		if (pTile)
			pTile->Render_GameObject();
	}
}

CTile* CTileMgr::Get_Tile(_int iX, _int iZ)
{
	for (auto& pTile : m_vecTiles)
	{
		// Search for tile at given grid position
		// This is a simple linear search; can be optimized with a map if needed
	}
	return nullptr;
}

void CTileMgr::Reset_For_SceneChange()
{
	for (auto& pTile : m_vecTiles)
	{
		Safe_Release(pTile);
	}
	m_vecTiles.clear();
}

void CTileMgr::Push_State()
{
	// 현재 상태를 스택에 보관
	TILE_STATE tState;
	tState.vecTiles = m_vecTiles;
	tState.iMapWidth = m_iMapWidth;
	tState.iMapHeight = m_iMapHeight;
	tState.bInitialized = m_bInitialized;

	m_stackState.push(tState);

	// 새 씬용으로 비움 (Release 안 함!)
	m_vecTiles.clear();
	m_iMapWidth = 0;
	m_iMapHeight = 0;
	m_bInitialized = false;
}

void CTileMgr::Pop_State()
{
	if (m_stackState.empty())
		return;

	// 현재 상태 정리 (미니게임 타일 등)
	for (auto& pTile : m_vecTiles)
	{
		Safe_Release(pTile);
	}
	m_vecTiles.clear();

	// 보관된 상태 복원
	TILE_STATE tState = m_stackState.top();
	m_stackState.pop();

	m_vecTiles = tState.vecTiles;
	m_iMapWidth = tState.iMapWidth;
	m_iMapHeight = tState.iMapHeight;
	m_bInitialized = tState.bInitialized;

	m_bSkipFirstFrame = true;
}

void CTileMgr::Free()
{
	for (auto& pTile : m_vecTiles)
	{
		Safe_Release(pTile);
	}
	m_vecTiles.clear();

	// 스택에 보관된 이전 씬 타일들도 정리
	while (!m_stackState.empty())
	{
		TILE_STATE& tState = m_stackState.top();
		for (auto& pTile : tState.vecTiles)
		{
			Safe_Release(pTile);
		}
		tState.vecTiles.clear();
		m_stackState.pop();
	}

	Safe_Release(m_pGraphicDev);
}
