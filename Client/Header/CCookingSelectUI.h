#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CCookingSelectUI :
	public CUi
{
private:
	explicit CCookingSelectUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCookingSelectUI();

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void    LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject();
	virtual void    OnCollision(CGameObject* pObject);

public:
	static CCookingSelectUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void SetRender(_bool _bRender) { m_bRender = _bRender; }

	void AddFood();
	_int Get_CookingCount() { return m_iCurCookingCount; }

private:
	virtual void Free();

private:
	vector<CGameObject*> m_vecCookingSelectUI;
	vector<CGameObject*> m_vecFoodSlot;

	_bool  m_bRender;

	_int   m_iCurCookingCount;
	_int   m_iCookingCountMax;

};
