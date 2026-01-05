#pragma once
#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CNode;
class CN2_AI;

class CMonsterN2 : public CMonster 
{
public:
	enum MONSTER_N2_STATE { N2S_CRAWL, N2S_JUMP, N2S_LAND, N2S_SPAWN, N2S_STOP, N2S_END };

private:
	explicit	CMonsterN2(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CMonsterN2(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CMonsterN2(const CMonsterN2& rhs);
	virtual		~CMonsterN2();

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
	_vec3				Compute_LimitedDir(const _float& fAngle, const _vec3& vCurDir, const _vec3& vDesiredDir);

private:
	// 스프라이트 관련
	MONSTER_N2_STATE	m_ePreState;
	MONSTER_N2_STATE	m_eCurState;
	_float				m_fFrame;
	_float				m_fFrameEnd;
	_float				m_fFrameSpeed;
	_matrix				m_matTex;

	// 스테이터스 관련
	_int		m_iAttack;

	// AI 관련
	CN2_AI*		m_pAICom;

	// 마디 관련
	CNode*		m_pNode[3];

public:
	static CMonsterN2*	Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void		Free();
};
