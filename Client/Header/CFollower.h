#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTextureSet;
	class CCollider;
}

class CFollower : public CGameObject
{
public:
	enum FOLLOWER_STATE {	FOLLOWER_IDLE,	FOLLOWER_RUN,		FOLLOWER_WOOD,
							FOLLOWER_ROCK,	FOLLOWER_PRAY,		FOLLOWER_BUILD,
							FOLLOWER_DANCE,	FOLLOWER_RECRUIT,	FOLLOWER_END };

private:
	explicit	CFollower(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CFollower(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CFollower(const CFollower& rhs);
	virtual		~CFollower();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject);

private:
	HRESULT				Add_Component();

	void				Ready_Variable();
	void				Ready_Event();

	void				Check_Frame();
	void				Move_Frame(const _float& fTimeDelta);
	void				Set_TextureSet();

	void				Update_State();

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTextureSet*	m_pTexSetCom;
	Engine::CCollider*		m_pColliderCom;

	_vec3				m_vPos;

	// 스프라이트 관련
	FOLLOWER_STATE	m_ePreState;
	FOLLOWER_STATE	m_eCurState;
	_float			m_fFrame;
	_float			m_fFrameEnd;
	_float			m_fFrameSpeed;
	_matrix			m_matTex;
	_uint			m_iTexIdx;
	wstring			m_strFrameKey[FOLLOWER_END];

	// 스테이터스 관련
	_float			m_fGroundY;

public:
	static CFollower* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void		Free();
};
