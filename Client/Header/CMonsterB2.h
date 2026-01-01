#pragma once
#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CB2_AI;

class CMonsterB2 : public CMonster 
{
public:
	enum MONSTER_B2_STATE { B2S_IDLE, B2S_RUN, B2S_ATTACK, B2S_HIT, B2S_SPAWN, B2S_JEER, B2S_PRAY, B2S_END };
	enum ATTACK_PHASE { PREPARE, EXECUTE };

private:
	explicit	CMonsterB2(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CMonsterB2(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CMonsterB2(const CMonsterB2& rhs);
	virtual		~CMonsterB2();

public:
	void				Set_Dir(_vec3* pDir)	{ m_vDir = *pDir; }
	const _vec3*		Get_Dir()				{ return &m_vDir; }

	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) {};

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
	MONSTER_B2_STATE	m_ePreState;
	MONSTER_B2_STATE	m_eCurState;
	_float				m_fFrame;
	_float				m_fFrameEnd;
	_float				m_fFrameSpeed;
	_vec3				m_vNormDir[DIR_END];
	_vec3				m_vDir;
	_matrix				m_matTex;
	ATTACK_PHASE		m_eAttackPhase;

	// 스테이터스 관련
	_int				m_iAttack;

	// AI 관련
	CB2_AI*				m_pAICom;

public:
	static CMonsterB2*	Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void		Free();
};
