#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CWeaponInfo :
	public CUi
{
public:
	enum WINFOTYPE {WINFO_SWORD, WINFO_GAUNTLET, WINFO_END};
private:
	explicit CWeaponInfo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWeaponInfo();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

	void			Active() { m_bActive = true; }
	void			UnActive() { m_bActive = false; }

public:
	static CWeaponInfo* Create(LPDIRECT3DDEVICE9 pGraphicDev, WINFOTYPE eType);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	WINFOTYPE m_eType = WINFO_END;

	_bool	 m_bActive = false;
};

