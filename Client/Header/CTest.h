#pragma once

#include "CScene.h"

class CGauge;

class CTest : public CScene
{
private:
	explicit CTest(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTest();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);

	//HRESULT			Ready_Const_Layer();
	HRESULT			Ready_Const_Layer(CLayer* pConstLayer);

	HRESULT			Ready_Light();

public:
	static CTest* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();

private:
	CGauge* m_pTestGauge;

};

