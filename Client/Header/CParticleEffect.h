#pragma once
#include "CEffect.h"

class CParticleEffect : public CEffect
{
public:
	struct Particle
	{
		_vec3   vPos;
		_vec3   vSpeed;
		_float  fLife;
		_float  fMaxLife;
		_float  fSize;
		_float  fAlpha;
		DWORD   dwColor;
	};

private:
	explicit	CParticleEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CParticleEffect(const CParticleEffect& rhs);
	virtual		~CParticleEffect();

public:
	virtual HRESULT	Ready_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject();

	// 필수
	virtual void    Play() override;
	virtual void    Stop() override;
	virtual void    Reset() override;

	// 선택
	virtual void    OnPlay() {}         // 재생 시작 시
	virtual void    OnStop() {}         // 정지 시
	virtual void    OnFinish() {}       // 완료 시
	virtual void    OnLoop() {}         // 루프 시

public:
	static CParticleEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _uint iTexIdx);

private:
	vector<Particle>	m_vecParticles;
	_uint				m_iMaxParticles;

	// 발생 설정
	_float      m_fEmitRate;		// 초당 발생 수
	_float      m_fEmitAcc;			// 발생 누적
	_vec3       m_vEmitMinPos;		// 발생 위치 범위
	_vec3       m_vEmitMaxPos;
	_vec3       m_vMinSpeed;		// 초기 속도 범위
	_vec3       m_vMaxSpeed;

	// 물리
	_vec3       m_vGravity;
	_float      m_fDrag;            // 공기 저항

	// 시각
	_float      m_fSizeStart;
	_float      m_fSizeEnd;
	_float      m_fAlphaStart;
	_float      m_fAlphaEnd;

private:
	virtual void	Free();
};
