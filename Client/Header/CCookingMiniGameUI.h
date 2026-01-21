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
	static CCookingMiniGameUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);
	_bool Check_CookingResult();
	
	void Set_Render(_bool _bRender) { m_bRender = _bRender; };
	_bool CookingInput();
	void CookingStart(_int CookingCount);
	void CookingEnd();


	//콜백 추가부분
	using CookingEndCallback = std::function<void()>;

	void Set_CookingEndCallback(CookingEndCallback cb)
	{
		m_OnCookingEnd = cb;
	}
private:
	CookingEndCallback m_OnCookingEnd;
	//

private:

	virtual			void		Free();

private:
	vector<CGameObject*> m_vecCookingUI;

	_bool		m_bRender;
	CCookingMarker* m_pMarker;
	CCookingGauge* m_pGauge;

	_int m_iCookingCount;
	_int m_iCurCookingCount;

	_float m_fMarkerStopTime;
	_float m_fMarkerCurStopTime;

	_bool  m_bInputLock;

};

