#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcColTitle;
}

class CTitleBack :
	public CUi
{
private:
	explicit CTitleBack(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTitleBack();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CTitleBack* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcColTitle* m_pBufferCom;
	CTransform* m_pTransformCom;
};

