#pragma once

#include "CScene.h"

class CCookingUIController;

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
	void			Ready_Event();

	void			Ready_Event_Village();

	void			Key_Input_Village();

public:
	static CVillage* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();

private:
	_bool	m_bLeshyDungeonFlag = false;
	_bool	m_bKnuckleBoneFlag = false;
	_bool	m_bCookingFlag = false;
	CCookingUIController* m_pCookingUI;
};
