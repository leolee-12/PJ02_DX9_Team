#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CKBBoardBack :
	public CUi
{
private:
	explicit CKBBoardBack(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKBBoardBack();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	void		Set_Select(_bool bSelect) { m_bSelect = bSelect; }
	void		Move_Right();
	void		Move_Left();
	void 		Move_Center();


public:
	static CKBBoardBack* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_bool	  m_bSelect;
};
