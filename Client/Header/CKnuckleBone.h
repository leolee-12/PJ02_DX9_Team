#pragma once
#include "CScene.h"

class CKBTab;

class CKnuckleBone : public CScene
{
public:
	enum KBSTATE { KB_TITLE, KB_TUTO, KB_MAIN, KB_END };
	enum KBTITLEOPTION { KBT_PLAY, KBT_HOWTOPLAY, KBT_EXIT, KBT_END };
private:
	explicit CKnuckleBone(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKnuckleBone();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_Tutorial_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_Title_Layer(const _tchar* pLayerTag);

	HRESULT			Ready_Light();
	void			Render_Font_Tutorial();
	void			Render_Font_Title();

	void			State_machine();
	void			Key_Input_KB();

public:
	static CKnuckleBone* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();

private:
	KBSTATE		m_eCurKBState;
	KBSTATE		m_ePreKBState;
	wstring		m_strLayerTag;

	CKBTab*		m_pTitleTab;

	KBTITLEOPTION m_eTitleOption;
};

