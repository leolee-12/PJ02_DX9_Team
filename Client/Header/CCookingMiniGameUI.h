#pragma once
#include "CUi.h"


namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CCookingMarker;
class CCookingGauge;


class CCookingMiniGameUI :
	public CUi
{
private:
	explicit CCookingMiniGameUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCookingMiniGameUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CCookingMiniGameUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	_bool Check_CookingResult();
	_bool CookingInput();

private:
	virtual			void		Free();


private:
	vector<CGameObject*> m_vecCookingUI;

	_bool		m_bRender;
	CCookingMarker* m_pMarker;
	CCookingGauge* m_pGauge;

	int m_iCookingCount;
	int m_iCurCookingCount;
};

