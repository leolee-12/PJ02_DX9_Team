#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CKBDiceBox :
	public CUi
{
private:
	explicit CKBDiceBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKBDiceBox();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CKBDiceBox* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iOwner);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	// 0 플, 1 NPC
	_int   m_iOwner;
	D3DXCOLOR m_tColor;
};

