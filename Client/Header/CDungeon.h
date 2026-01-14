#pragma once

#include "CScene.h"

#include "CPlayer.h"
#include "CMonster.h"
#include "CTerrain.h"
#include "CTerrainWall.h"

class CGauge;
class CCookingMiniGameUI;
class CCookingSelectUI;

class CDungeon : public CScene
{
private:
	explicit CDungeon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDungeon();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Const_Layer();

	HRESULT			Ready_Light();
	void			Ready_Event();

public:
	static CDungeon* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();

private:
	CGauge*		 m_pGauge;

};

