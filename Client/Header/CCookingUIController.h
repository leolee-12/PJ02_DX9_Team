#pragma once
#include "CUi.h"

class CCookingSelectUI;
class CCookingMiniGameUI;

class CCookingUIController :
	public CUi
{
private:
	explicit CCookingUIController(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCookingUIController();

public:
	enum COOKINGUISTATE
	{
		CS_IDLE,
		CS_SELECT,
		CS_MINIGAME,
		CS_COOKINGEND,
		CS_END
	};

public:
	virtual HRESULT Ready_GameObject();
	virtual _int    Update_GameObject(const _float& fTimeDelta);
	virtual void    LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void    Render_GameObject();
	virtual void    OnCollision(CGameObject* pObject);

public:
	static CCookingUIController* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);

	void Set_State(COOKINGUISTATE eState) { m_eState = eState; }
	COOKINGUISTATE Get_State() const { return m_eState; }

	void Start_Cooking(_int iCookingCount);
	void Set_CookingState(COOKINGUISTATE cs) { m_eState = cs; }
private:
	virtual void Free();

	void	Key_Input_Cooking();
	void	State_Machine();

private:
	CCookingSelectUI* m_pSelectUI;
	CCookingMiniGameUI* m_pMiniGameUI;

	COOKINGUISTATE m_eState;
	COOKINGUISTATE m_ePrevState = CS_END;
};
