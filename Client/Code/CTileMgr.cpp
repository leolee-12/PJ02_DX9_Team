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

	// 맵 데이터로부터 타일 생성
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

	return S_OK;
}

_int CTileMgr::Update(const _float& fTimeDelta)
{
	if (!m_bInitialized)
		return 0;

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

void CTileMgr::Free()
{
	for (auto& pTile : m_vecTiles)
	{
		Safe_Release(pTile);
	}
	m_vecTiles.clear();

	Safe_Release(m_pGraphicDev);
}
