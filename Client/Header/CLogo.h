#pragma once
#include "CScene.h"
#include "CLoadingThread.h"

class CTitleTab;

class CLogo : public CScene
{
public:
	enum LOGOSTATE { LS_START, LS_EDIT, LS_EXIT, LS_END };
private:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);

	HRESULT			Ready_Light();

private:
	HRESULT			Ready_Prototype();
	void			Key_Input_Logo();

private:
	CLoadingThread* m_pLoading;
	CTitleTab*		m_pTitleTab;
	LOGOSTATE		m_eLogoState = LS_END;


public:
	static CLogo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();

};

