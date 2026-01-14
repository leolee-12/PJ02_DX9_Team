#pragma once
#include "CAIController.h"
#include "CMonsterB2.h"

class CB2_AI : public CAIController
{
private:
	typedef struct tagB2AttackPattern
	{
		CMonsterB2::MONSTER_B2_STATE eType;
		int iWeight;
		bool bIsActive;  // 페이즈별 활성화
	}B2_ATKPATTERN;

protected:
	explicit	CB2_AI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CB2_AI(const CB2_AI& rhs);
	virtual		~CB2_AI();

protected:
	HRESULT		Ready_AI(const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState);
	void		Enter_State(const _uint& iState)	override;
	void		Exit_State(const _uint& iState)		override;
	void		Generate_Pattern(CMonsterB2::MONSTER_B2_STATE eState, _bool bAllowDuplicate = false);
	void		Refill_Pattern(_bool bAllowDuplicate = false);

public:
	void		Set_Owner(CMonsterB2* pOwner) { m_pOwner = pOwner; }
	void		Set_Speed(const _float& fSpeed) { m_fSpeed = fSpeed; }
	void		Anim_End(CMonsterB2::MONSTER_B2_STATE eState);
	void		Push_Front_Pattern(CMonsterB2::MONSTER_B2_STATE eState);
	void		Set_Weight(CMonsterB2::MONSTER_B2_STATE eState, _uint iNewWeight);
	
	void		Set_Signal(_uint iNum = 0);
	// Owner가 특정 순간에 AI에게 신호를 보내는 용도

private:
	_int		Update_Component(const _float& fTimeDelta)	override;
	void		Update_Idle(const _float& fTimeDelta);
	void		Update_Dig(const _float& fTimeDelta);
	void		Update_Escape(const _float& fTimeDelta);
	void		Update_Hit(const _float& fTimeDelta);
	void		Update_Smash(const _float& fTimeDelta);
	void		Update_Shoot(const _float& fTimeDelta);
	void		Update_Summon(const _float& fTimeDelta);
	void		Update_Spawn(const _float& fTimeDelta);
	void		Update_Die(const _float& fTimeDelta);
	void		Update_Dead(const _float& fTimeDelta);
	void		Update_Jump(const _float& fTimeDelta);
	void		Update_Dive(const _float& fTimeDelta);
	void		Update_Spike1(const _float& fTimeDelta);
	void		Update_Spike2(const _float& fTimeDelta);

private:

	_float		m_fSpeed;
	_vec3		m_vSpeed;
	_float		m_fAngle;
	_float		m_fGravity;
	_float		m_fAcmlTime;
	_bool		m_bChase;
	_vec3		m_vLerpPos;		// Lerp용 위치

	// 패턴 관련
	deque<CMonsterB2::MONSTER_B2_STATE> m_patternDeque;
	vector<B2_ATKPATTERN>	m_vecAtkPatterns;
	_uint		m_iDequeMinSize;
	CMonsterB2*	m_pOwner;
	_bool		m_bOnce;	// 패턴용 함수가 단 한 번만 실행되도록 제어
	_uint		m_iSwitch;	// 패턴을 위해 Owner로부터 전달받을 다목적용 값

public:
	static CB2_AI*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState = 0);
	CComponent*		Clone()	override;

protected:
	void			Free()	override;
};
