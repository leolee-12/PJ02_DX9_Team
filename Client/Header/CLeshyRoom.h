#pragma once

#include "CScene.h"

class CLeshyRoom : public CScene
{
private:
	explicit CLeshyRoom(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLeshyRoom();

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

public:
	static CLeshyRoom* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();
};
