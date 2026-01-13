#pragma once

#include "CScene.h"

class CVillage : public CScene
{
private:
	explicit CVillage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CVillage();

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

public:
	static CVillage* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();
};
