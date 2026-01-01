#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CItem : public CGameObject
{
public:
	enum ITEMSTATE { IS_SPAWN, IS_IDLE, IS_CHASE, IS_END };
	enum ITEMID {	IG_GOLD, IG_WOOD, IG_STONE, IG_BERRY, IG_FERTILIZER, IG_FERVOR,
					FD_GFOOD, FD_BFOOD,
					WP_SWORD, WP_GAUNTLET, WP_FIREBALL, WP_TENTACLE,
					ID_END };

protected:
	explicit	CItem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CItem(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CItem(const CItem& rhs);
	virtual		~CItem();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) {};

protected:
	HRESULT				Add_Component();

	void				Ready_Variable();
	void				Ready_Event();
	void				Check_State();
	void				Set_Texture();

	void				Update_Spawn(const _float& fTimeDelta);
	void				Update_Idle(const _float& fTimeDelta);
	void				Update_Chase(const _float& fTimeDelta);

protected:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCollider*	m_pColliderCom;

	_vec3				m_vPos;
	_vec3				m_vDir;

	// 상태 관련
	ITEMSTATE			m_ePreState;
	ITEMSTATE			m_eCurState;
	ITEMID				m_eItemID;
	_float				m_fAcmlTime;

	// 물리 관련
	_vec3				m_vSpeed;
	_float				m_fGravity;
	_float				m_fBounceDamp;
	_float				m_fGroundY;

public:
	static CItem* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, const _vec3& vPos, ITEMID eID);

protected:
	virtual void Free();
};