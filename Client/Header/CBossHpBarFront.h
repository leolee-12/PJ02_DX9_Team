#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CBossHpBarFront :
	public CUi
{
private:
	explicit CBossHpBarFront(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBossHpBarFront();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CBossHpBarFront* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale);

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec3 m_vPos;
	float m_fScale;
};

