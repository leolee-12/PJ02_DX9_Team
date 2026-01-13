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
	void		Generate_Pattern(CMonsterB2::MONSTER_B2_STATE eState);
	void		Refill_Pattern();

public:
	void		Set_Owner(CMonsterB2* pOwner) { m_pOwner = pOwner; }
	void		Set_Speed(const _float& fSpeed) { m_fSpeed = fSpeed; }
	void		Anim_End(CMonsterB2::MONSTER_B2_STATE eState);
	void		Push_Front_Pattern(CMonsterB2::MONSTER_B2_STATE eState);
	void		Set_Weight(CMonsterB2::MONSTER_B2_STATE eState, _uint iNewWeight);
private:
	_int		Update_Component(const _float& fTimeDelta)	override;
	void		Update_Crawl(const _float& fTimeDelta);
	void		Update_Jump(const _float& fTimeDelta);
	void		Update_Land(const _float& fTimeDelta);
	void		Update_Prepare(const _float& fTimeDelta);
	void		Update_Attack(const _float& fTimeDelta);
	void		Update_Shoot(const _float& fTimeDelta);
	void		Update_Summon(const _float& fTimeDelta);
	void		Update_Roar(const _float& fTimeDelta);
	void		Update_Spawn(const _float& fTimeDelta);
	void		Update_Stop(const _float& fTimeDelta);

private:

	_float		m_fSpeed;
	_vec3		m_vSpeed;
	_float		m_fAngle;
	_float		m_fGravity;
	_float		m_fAcmlTime;
	_bool		m_bChase;
	_vec3		m_vLerpPos;		// Lerp용 위치

	// 패턴 관련
	deque<CMonsterB2::MONSTER_B2_STATE> m_patternDeque;		// 공격 패턴을 담을 덱
	vector<B2_ATKPATTERN>				m_vecAtkPatterns;	// 공격 패턴들의 정보(상태, 빈도, 활성화 여부)
	_uint								m_iDequeMinSize;	// 덱에 담을 패턴의 최소 개수
	CMonsterB2* m_pOwner;			// 메시지 발행은 Owner만 가능(컴포넌트는 메시지채널이 없어 불가)
	_bool								m_bOnce;			// 패턴 관련 로직이 딱 한 번만 실행되도록

	//-------------------------<패턴덱 사용 법>--------------------------
	// - front()로 다음 패턴 가져온 뒤 pop_front()
	// - back()으로 마지막 패턴 확인하여 중복아닌 다음 패턴을 push_back()
	// - 강제 패턴 삽입은 push_front()
	//-------------------------------------------------------------------

public:
	static CB2_AI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _float& fDetectRange, const _float& fInteractRange, const _uint& iInitState = 0);
	CComponent* Clone()	override;

protected:
	void				Free()	override;
};