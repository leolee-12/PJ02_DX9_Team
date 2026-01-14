#pragma once
#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CN1_AI;

class CMonsterN1 : public CMonster 
{
public:
	enum MONSTER_N1_STATE { N1S_IDLE, N1S_RUN, N1S_ATTACK, N1S_HIT, N1S_SPAWN, N1S_JEER, N1S_PRAY, N1S_END };
	enum ATTACK_PHASE { PREPARE, EXECUTE };

private:
	explicit	CMonsterN1(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CMonsterN1(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CMonsterN1(const CMonsterN1& rhs);
	virtual		~CMonsterN1();

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
	void				Set_Texture();

	void				Attack_HitBox();
	void				Attacked(const _int& iAttack);
	void				Update_State();

private:
	// 스프라이트 관련
	MONSTER_N1_STATE	m_ePreState;
	MONSTER_N1_STATE	m_eCurState;
	_float				m_fFrame;
	_float				m_fFrameEnd;
	_float				m_fFrameSpeed;
	_matrix				m_matTex;
	ATTACK_PHASE		m_eAttackPhase;	// SubState

	// AI 관련
	CN1_AI*				m_pAICom;

public:
	static CMonsterN1*	Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void		Free();
};
