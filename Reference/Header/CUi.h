#pragma once
#include "CGameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUi : public CGameObject
{
protected:
	explicit CUi(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CUi(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);
	virtual ~CUi();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

protected:
	virtual		void		Free();

	void					Compute_ViewDepth_Ortho(const _vec3* pPos);

protected:
	_vec3		m_vPos;
};

END

