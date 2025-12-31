#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcCol;
}

class CLoadingBack :
    public CUi
{
private:
	explicit CLoadingBack(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoadingBack();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CLoadingBack* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcCol*		m_pBufferCom;
	CTransform*	m_pTransformCom;
};

