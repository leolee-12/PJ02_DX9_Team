#pragma once
#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CN3_AI;

class CMonsterN3 : public CMonster 
{
public:
	enum MONSTER_N3_STATE { N3S_FLY, N3S_PREPARE, N3S_RUSH, N3S_SPAWN, N3S_STOP, N3S_END };

private:
	explicit	CMonsterN3(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CMonsterN3(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CMonsterN3(const CMonsterN3& rhs);
	virtual		~CMonsterN3();

public:
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
	MONSTER_N3_STATE	m_ePreState;
	MONSTER_N3_STATE	m_eCurState;
	_float				m_fFrame;
	_float				m_fFrameEnd;
	_float				m_fFrameSpeed;
	_matrix				m_matTex;

	// 스테이터스 관련
	_int				m_iAttack;

	// AI 관련
	CN3_AI*				m_pAICom;

public:
	static CMonsterN3*	Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void		Free();
};
