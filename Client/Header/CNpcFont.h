#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcCol;
}

class CKBBack :
	public CUi
{
private:
	explicit CKBBack(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKBBack();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CKBBack* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcCol* m_pBufferCom;
	CTransform* m_pTransformCom;


};

