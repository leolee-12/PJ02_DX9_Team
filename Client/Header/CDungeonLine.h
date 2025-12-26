#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CDungeonLine :
	public CUi
{
public:
	enum DLINESTATE { DL_1, DL_2, DL_END };
private:
	explicit CDungeonLine(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDungeonLine();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CDungeonLine* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pChannel, DLINESTATE eState);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex*		m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture*	m_pTextureCom;

	_matrix		m_matUV;

	_vec3		m_vPos;
	_uint		m_fFrame = 0;
	_bool		m_bRender;
	_bool		m_bAnim;
	DLINESTATE  m_eState;
	_vec3		m_vColor;
	D3DMATERIAL9 m_tMtrl;
};

