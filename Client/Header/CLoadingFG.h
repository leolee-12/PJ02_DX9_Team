#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CLoadingFG :
	public CUi
{
private:
	explicit CLoadingFG(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoadingFG();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CLoadingFG* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	void			   Update_Pos(_float LoadingPercent);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec3		m_vPos;
};

