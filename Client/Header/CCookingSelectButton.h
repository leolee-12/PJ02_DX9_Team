#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CCookingSelectButton :
	public CUi
{
private:
	explicit CCookingSelectButton(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCookingSelectButton();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);


public:
	static CCookingSelectButton* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iPage, _vec3 vPos, _float fScale);

	void Changeimage(_int ipage) { m_iPage = ipage; }
private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();


private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec3 m_vPos;
	_float m_fScale;
	_int m_iPage;
};

