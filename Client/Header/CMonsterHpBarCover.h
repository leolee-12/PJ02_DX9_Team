#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CMonsterHpBarCover :
	public CUi
{
private:
	explicit CMonsterHpBarCover(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonsterHpBarCover();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	void			Set_Pos(const _vec3& vPos);

public:
	static CMonsterHpBarCover* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec3 m_vPos;
};

