#pragma once

#include "CScene.h"

class CFontUIOrtho;
class CSpeechBubbleOrtho;

class CTheGateway : public CScene
{
private:
	explicit CTheGateway(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTheGateway();

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

	void			Select_Key_Input();

public:
	static CTheGateway* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();

private:
	CFontUIOrtho* m_pLeftSelect;
	CFontUIOrtho* m_pRightSelect;

	CSpeechBubbleOrtho* m_pSpeechBubble;

	_bool		  m_bSelect = false;
	_uint		  m_iSelectSlot = 0;
};
