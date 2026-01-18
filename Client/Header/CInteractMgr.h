#pragma once
#include "CBase.h"
#include "Engine_Define.h"

class CInteractMgr : public CBase
{
	DECLARE_SINGLETON(CInteractMgr)

public:
	enum INTERACT_TYPE { NONE, WOOD, ROCK, BUILD, FOOD, PRAY, INTERACT_END };

private:
	explicit CInteractMgr();
	virtual ~CInteractMgr();

public:
	HRESULT	Ready_InteractMgr(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::MAPDATA& mapData);

private:
	virtual void Free();

private:
	_bool m_bSkipFirstFrame = false;
};
