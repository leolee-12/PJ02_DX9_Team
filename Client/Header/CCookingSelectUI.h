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
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CCookingSelectUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual			void		Free();


private:
	vector<CGameObject*> m_vecCookingSelectUI;

	_bool		m_bRender;
	float m_iCurCookingCount;
	float m_iCookingCount;
};

