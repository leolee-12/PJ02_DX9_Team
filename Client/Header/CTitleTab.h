#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CTitleTab :
	public CUi
{
private:
	explicit CTitleTab(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTitleTab();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CTitleTab* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void			  Move_Down();
	void			  Move_Up();

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec3		m_vPos;
};

