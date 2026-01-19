#pragma once

#include "CScene.h"

class CRealDungeon : public CScene
{
private:
	explicit CRealDungeon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRealDungeon();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);

	HRESULT			Ready_Light();
	void			Ready_Event();

public:
	static CRealDungeon* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();
};
