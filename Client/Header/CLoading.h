#pragma once
#include "CScene.h"
#include "CLoadingThread.h"


class CLoadingFG;

class CLoading : public CScene
{
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID);
	virtual ~CLoading();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);

private:
	HRESULT			Ready_Prototype();

private:
	CLoadingThread* m_pLoading;
	LOADINGID		m_eChangeID;
	CLoadingFG*		m_pLoadingFG;

public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID);
private:
	virtual void Free();

};

