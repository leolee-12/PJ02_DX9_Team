#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}


class CPlayerWeaponUI :
	public CUi
{
private:
	explicit CPlayerWeaponUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayerWeaponUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	void			Set_WeaponType(_int iType) { m_iPage = iType; }

public:
	static CPlayerWeaponUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, _float _fScale, _int _iPage);

private:
	virtual			void		Free();
	void						Ready_Event();
	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_int	m_iPage;

	_bool m_bRender;

	_vec3 m_vPos;
	_float m_fScale;

};

