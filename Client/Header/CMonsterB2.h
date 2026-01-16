#pragma once
#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTextureSet;
	class CCollider;
}

class CNode;
class CB2_AI;
class CProjectile;

class CMonsterB2 : public CMonster
{
public:
	enum MONSTER_B2_STATE {	B2S_IDLE,	B2S_DIG,	B2S_ESCAPE,
							B2S_HIT,	B2S_SMASH,	B2S_SHOOT,
							B2S_SUMMON,	B2S_SPAWN,	B2S_DIE,
							B2S_DEAD,	B2S_JUMP,	B2S_DIVE,
							B2S_SPIKE1,	B2S_SPIKE2,	B2S_END };
	// 상태		폴더명
	// DIG		move-out
	// ESCAPE	move-in
	// HIT		hurt
	// SMASH	head-smash
	// SHOOT	trunk-strike
	// SPAWN	transform
	// DIE		die (죽음 애니메이션)
	// DEAD		dead (죽어있는 상태(die 이후))

private:
	explicit	CMonsterB2(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CMonsterB2(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CMonsterB2(const CMonsterB2& rhs);
	virtual		~CMonsterB2();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject);

	void				Launch_Projectile(const _uint& iCount, const _vec3& vTargetDir);
	void				Summon_Minion(const _uint& iCount);
	void				Attack_HitBox(_vec3 vPos);
	void				Summon_Spike(const _uint& iCount, const _vec3& vSpeed = _vec3{0.f, 0.f, 0.f});


private:
	HRESULT				Add_Component();

	void				Ready_Variable();
	void				Ready_Event();

	void				Check_Frame();
	void				Move_Frame(const _float& fTimeDelta);
	void				Set_TextureSet();
	void				Set_Material();
	void				Reset_Material();

	void				Attacked(const _int& iAttack);
	void				Update_State();
	_vec3				Compute_LimitedDir(const _float& fAngle, const _vec3& vCurDir, const _vec3& vDesiredDir);
	void				Check_Phase();
	void				Check_Status();
private:
	// 스프라이트 관련
	CTextureSet*		m_pTexSetCom;
	wstring				m_strFrameKey;
	MONSTER_B2_STATE	m_ePreState;
	MONSTER_B2_STATE	m_eCurState;
	_float				m_fFrame;
	_float				m_fFrameEnd;
	_float				m_fFrameSpeed;
	//_matrix				m_matTex;	// 거대한 보스여서 Flip 안해도 괜찮아보임
	_float				m_fBtmPadding;

	// AI 관련
	CB2_AI*	m_pAICom;

	// 패턴 관련
	_uint			m_iPhase;
	_uint			m_iMaxHp;
	_bool			m_bMtrl = false;
	_float			m_fAcmlTime;

public:
	static CMonsterB2* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	static CMonsterB2* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos);

private:
	virtual void		Free();
};
