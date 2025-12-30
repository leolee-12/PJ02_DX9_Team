#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CDivider :
	public CUi
{
private:
	explicit CDivider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDivider();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CDivider* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iPage, const _vec3& vPos);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_uint m_iPage;
};

