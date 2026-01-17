#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTextureSet;
	class CCalculator;
	class CCollider;
}

class CTriggerPoint;

class CPlayer : public CGameObject
{
public:
	enum PLAYERSTATE { PS_IDLE, PS_RUN, PS_ROLL, PS_ATTACK, PS_CHARGE, PS_HIT, PS_ACTION, PS_REBIRTH, PS_END };

	// ==========================
	//	PLAYERSTATE : 플레이어 상태 관리용 enum
	//	DIRECTIONID : 플레이어 및 각종 방향 관리용 enum 
	// ==========================

private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

public:
	void			Set_Pos(const _vec3& vPos);
	void			Set_Intro(_bool bUseIntro) { m_bIntro = bUseIntro; }

	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

private:
	void			Ready_Variable();
	void			Ready_Event();

	HRESULT			Add_Component();
	void			Key_Input(const _float& fTimeDelta);
	void			Set_OnTerrain();
	_vec3			Picking_OnTerrain();

	// ==========================
	//	Key_Input : WSAD 이동 / SPACE 구르기 / LBUTTON 공격 및 상호작용
	// ==========================

	void			Check_Frame();
	void			Move_Frame(const _float& fTimeDelta);
	void			Set_TextureSet();
	void			Set_FrameKey();
	void			Set_Size();
	// ==========================
	//	Check_Frame : 상태 변경 시 Frame 초기화 및 설정해줄 값을 대입해줌
	// 	Move_Frame : 시간 경과에 따라 Frame 값 누적, 특정 상태는 스프라이트 종료 시 IDLE 상태로 돌아가도록 세팅됨
	//  Set_TextureSet : Set_Texture와 같은 목적, 상태값까지 TextureSet 컴포넌트에 전달하여 알맞는 텍스처가 세팅되도록 함
	// ==========================

	void			Move_Roll(const _float& fTimeDelta);
	void			Charge(const _float& fTimeDelta);
	void			Attack_HitBox();

	// ==========================
	//	Move_Roll : 구르기 상태일 때 현재 위치를 Lerp를 적용하여 계산 및 이동
	// ==========================

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTextureSet*	m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;
	Engine::CCollider*		m_pColliderCom;

	// 스프라이트 관련
	PLAYERSTATE		m_ePreState;
	PLAYERSTATE		m_eCurState;
	wstring			m_strFrameKey;
	_float			m_fFrame;
	_float			m_fFrameEnd;
	_float			m_fFrameSpeed;
	_matrix			m_matTex;

	// 캐릭터 스테이터스 관련
	_vec3			m_vNormDir[DIR_END];
	_vec3			m_vDir;
	_float			m_fSpeed;
	_int			m_iAttack;
	_bool			m_bRoll;	// 구르기 중인지?
	_int			m_iCombo;	// 공격 중인지? + 몇번째 콤보상태인지?
	_bool			m_bAction;	// Action 중인지?

	// 알파 소팅 관련
	_vec3			m_vPos;

	// 구르기 관련
	_vec3			m_vRollPos;		// Lerp용 위치
	_float			m_fLerp;		// Lerp용 값

	// 차지 공격 관련
	_float			m_fCharge;
	_float			m_fChargeMax;

	// 메시지 채널 관련
	_bool 			m_bMsgRegistered;

	// 트리거 관련
	_bool			m_bCanTrigger;
	CTriggerPoint*	m_pTriggerPoint = nullptr;
	_bool			m_bIntro;			// Ready에서 false 처리 (Intro 스프라이트 사용하려면 Ready 이후 Set 해주기)
	_uint			m_iInteractType;	// 상호작용하는 대상 구분용

	// 캐릭터 border 처리 관련
	_vec3 m_vPrevPos;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void Free();

};

