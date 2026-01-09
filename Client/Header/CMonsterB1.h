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
class CB1_AI;

class CMonsterB1 : public CMonster
{
public:
	enum MONSTER_B1_STATE { B1S_CRAWL, B1S_JUMP, B1S_LAND, B1S_PREPARE, B1S_ATTACK, B1S_SHOOT, B1S_SPAWN, B1S_ROAR, B1S_STOP, B1S_END };

private:
	explicit	CMonsterB1(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CMonsterB1(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CMonsterB1(const CMonsterB1& rhs);
	virtual		~CMonsterB1();

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
	void				Compute_NodePos(const _float& fTimeDelta);

private:
	// 스프라이트 관련
	MONSTER_B1_STATE	m_ePreState;
	MONSTER_B1_STATE	m_eCurState;
	_float				m_fFrame;
	_float				m_fFrameEnd;
	_float				m_fFrameSpeed;
	_matrix				m_matTex;

	// 스테이터스 관련
	_int	m_iAttack;

	// AI 관련
	CB1_AI* m_pAICom;

	// 마디 관련
	CNode*		m_pNode[4];
	_float		m_fGroundY;

public:
	static CMonsterB1* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void		Free();
};
