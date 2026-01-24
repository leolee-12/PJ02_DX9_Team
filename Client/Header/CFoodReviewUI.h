#pragma once
#include "CUi.h"


namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CFoodReviewBack;
class CFoodReviewImage;
class CFontUIOrtho;

class CFoodReviewUI :
	public CUi
{
private:
	explicit CFoodReviewUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFoodReviewUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);
		

public:
	static CFoodReviewUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);
public:

private:
	virtual			void		Free();
	void						ReviewGoodFood();
	void						ReviewBadFood();
	void						Ready_Event();
private:
	CFoodReviewBack* m_pReviewBackUI;
	CFoodReviewImage* m_pReviewImageUI;
	CFontUIOrtho* m_pAHNReivewFont;
	CFontUIOrtho* m_pBackReivewFont;

	_bool				m_bActive;
	_float				m_fRenderTimer;
	wstring				m_szSoundText;
	_bool				m_soundPlayed;
	
};

