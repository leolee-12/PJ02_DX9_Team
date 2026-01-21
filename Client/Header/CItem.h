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
	enum ITEMSTATE { IS_SPAWN, IS_IDLE, IS_CHASE, IS_SUMMON, IS_END };
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
	ITEMSTATE			Get_State() { return m_eCurState; }

	virtual HRESULT		Ready_GameObject();
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);	// 아직은 동일하므로 추상클래스에 넣었으나, 달라지면 각자 override하여 사용
	virtual void		Render_GameObject();								// 아직은 동일하므로 추상클래스에 넣었으나, 달라지면 각자 override하여 사용
	virtual void		OnCollision(CGameObject* pObject) PURE;				// 하위클래스에 구현 강제

	void				Ready_Variable();
	void				Ready_Event();
	void				Check_State();
	void				Update_Spawn(const _float& fTimeDelta);

protected:
	HRESULT				Add_Component();

protected:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCollider*	m_pColliderCom;

	_vec3				m_vPos;

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
	_float				m_fThrowRange;

public:
	static CItem* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, const _vec3& vPos, ITEMID eID, _bool isActive, _float fThrowRange = 1.f);

protected:
	virtual void Free();
};
