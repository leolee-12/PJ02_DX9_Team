#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

enum DICESTATE { DS_IDLE, DS_ROLLING, DS_SHOWING, DS_MOVING, DS_END };

class CKBDice :
	public CUi
{
private:
	explicit CKBDice(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKBDice();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	// Getter/Setter
	_int			GetValue() const { return m_iValue; }
	void			SetValue(_int iValue) { m_iValue = iValue; }
	DICESTATE		GetState() const { return m_eState; }

	// Dice Functions
	void			Roll();								// Start rolling (DS_ROLLING)
	void			ShowResult(_int iFinalValue);		// Show result (DS_SHOWING)
	void			MoveTo(const _vec3& vTargetPos);	// Start moving to target (DS_MOVING)
	void			SetPosition(const _vec3& vPos);		// Set position immediately
	void			SetDead() { m_bDead = true; }		// Mark for deletion

	void            Set_ColorWhite() { m_tColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f); }
	void            Set_ColorBlue() { m_tColor = D3DXCOLOR(0.7f, 0.9f, 1.f, 1.f); }
	void            Set_ColorYellow() { m_tColor = D3DXCOLOR(1.f, 1.f, 0.7f, 1.f); }

public:
	static CKBDice* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iTurn);

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	void						Update_Rolling(const _float& fTimeDelta);
	void						Update_Moving(const _float& fTimeDelta);
	void						Set_Rolling_Motion();
	void						Rolling_Motion(const _float& fTimeDelta);

private:
	// 컴포넌트
	CRcTex*			m_pBufferCom;
	CTransform*		m_pTransformCom;
	CTexture*		m_pTextureCom;

	// 주사위 데이터
	_int			m_iValue;			// 주사위 값 (1~6)
	DICESTATE		m_eState;			// 현재 상태

	// 굴리기 관련
	_float			m_fRollTime;		// 굴리기 경과 시간
	_float			m_fRollDuration;	// 굴리기 총 시간 (1.0초)
	_float			m_fRollInterval;	// 텍스쳐 변경 간격 (0.1초)
	_float			m_fRollAccum;		// 간격 누적 시간

	// 이동관련 (Lerp)
	_vec3			m_vStartPos;		// 이동 시작 위치
	_vec3			m_vTargetPos;		// 도착 위치
	_float			m_fMoveTime;		// 이동 경과 시간
	_float			m_fMoveDuration;	// 이동 총 시간 (0.5초)

	// 삭제 플래그
	_bool			m_bDead;			// true면 다음 Update에서 삭제

	// 굴린 사람
	_int			m_iTurn;			// 0=플레이어, 1=NPC

	// 통통튀는 애니매이션 관련
	_float          m_fBounceDemp;      // 감쇠 계수
	_float          m_fBounceSpeed;     // 튀는 속도
	_float          m_fBounceAccel;     // 가속도
	_float 			m_fBounceGoal;      // 도착지점

	// 주사위 색 관련
	D3DXCOLOR       m_tColor;
};

