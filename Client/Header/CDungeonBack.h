#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcCol;
}

class CDungeonBack :
	public CUi
{
private:
	explicit CDungeonBack(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDungeonBack(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pChannel);
	virtual ~CDungeonBack();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CDungeonBack* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pChannel);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcCol* m_pBufferCom;
	CTransform* m_pTransformCom;

	_bool		m_bRender;
};

