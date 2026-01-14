#pragma once
#include "CUi.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CPlayerHP :
	public CUi
{
private:
	explicit CPlayerHP(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerHP(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pChannel);
	virtual ~CPlayerHP();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CPlayerHP* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pChannel);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom_Full;
	//CTexture* m_pTextureCom_Half;

	_bool		m_bRender;

	_int		m_iPlayerHp;
	_int		m_iPlayerMaxHp;
};

