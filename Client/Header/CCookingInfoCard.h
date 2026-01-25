#pragma once
#include "CUi.h"


namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CFontUIOrtho;

class CCookingInfoCard :
	public CUi
{
private:
	explicit CCookingInfoCard(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCookingInfoCard();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CCookingInfoCard* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);
	void Init_CalcuResouceCount(_int _iCount);
	void Set_RecourceCountText(_int _iCount);
	void ApplayRecourceCountText(_int _iCount);
	_int Get_RecipeCount() const { return m_iRecipeCount; }
private:
	virtual			void		Free();
	void						Ready_Event();

private:
	vector<CGameObject*> m_vecCookingSelectUI;
	CFontUIOrtho*	m_pResourceFont;
	CFontUIOrtho*	m_pRecipeFont;

	_bool			m_bRender;
	_int			m_iRecipeCount;
	_int			m_iResourceCount;
};

