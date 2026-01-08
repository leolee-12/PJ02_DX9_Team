#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CKBMask :
	public CUi
{
private:
	explicit CKBMask(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKBMask();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

	void			Set_Index(_int iIndex) { m_iIndex = iIndex; }
	void            Show_Mask() { m_bRender = true; }

public:
	static CKBMask* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_int      m_iIndex;
	_bool     m_bRender;
};

