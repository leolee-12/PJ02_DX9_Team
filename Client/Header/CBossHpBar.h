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


class CBossHpBar :
	public CUi
{
private:
	explicit CBossHpBar(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBossHpBar();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CBossHpBar* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	_bool Check_CookingResult();

	void Set_Render(_bool _bRender) { m_bRender = _bRender; };


private:

	virtual			void		Free();

private:
	vector<CGameObject*> m_vecHpBarUI;

	_bool		m_bRender;
};

