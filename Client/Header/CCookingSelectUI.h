#pragma once
#include "CUi.h"


namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CCookingTargetFood;
class CCookingSelectButton;
class CCookingSelectSlot;
class CCookingInfoCard;

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
	static CCookingSelectUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);
	void SetRender(_bool _bRender) { m_bRender = _bRender; }

	void AddFood();
	void DeleteFood();
	void ReSetSelecting();
	void Set_CookingCount(_int num) { m_iCurCookingCount = num; }
	_int Get_CookingCount() { return m_iCurCookingCount; }
	void init_data();
	_int Get_RecipeCount();
private:
	virtual void Free();

private:
	vector<CGameObject*>		m_vecCookingSelectUI;


	vector<CCookingSelectSlot*>		m_vecFoodSlot;
	vector<CCookingTargetFood*>		m_vecFood;

	CCookingTargetFood*		m_pCookingtargetFood;
	CCookingSelectButton*	m_pCookingBtn;
	CCookingInfoCard*		m_pCookingInfoCard;

	_bool  m_bRender;

	_int   m_iCurCookingCount;
	_int   m_iCookingCountMax;

	_int m_iResourceCount;
};
