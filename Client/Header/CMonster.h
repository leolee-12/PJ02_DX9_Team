#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CMonster : public CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) {};

protected:
	HRESULT			Add_Component();

protected:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCollider*	m_pColliderCom;

	_vec3				m_vPos;

	// 스테이터스 관련
	_int				m_iAttack;
	_float				m_fScale;
	_float				m_fGroundY;

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

protected:
	virtual void Free();
};

