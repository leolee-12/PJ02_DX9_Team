#pragma once
#include "CEffect.h"

class CParticleEffect : public CEffect
{
public:
	struct Particle
	{
		_vec3		vPos;
		_vec3		vSpeed;
		_float		fLife;
		_float		fMaxLife;
		_float		fSize;
		_float		fAlpha;
		_uint		iTexIdx;
		D3DXCOLOR	tColor;
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

	void			Emit_Particle();

	void			Set_TextureKey(wstring strKey) { m_strProtoTexKey = strKey; }
	void			Set_EmitRange(const _vec3& v1, const _vec3& v2) {}
	void			Set_SpeedRange(const _vec3& v1, const _vec3& v2) {}
	void			Set_LifeTime(const _float& fTime) { m_fLifeTime = fTime; }
	void			Set_EmitRate(const _float& fRate) { m_fEmitRate = fRate; }
	void			Set_BaseColor(const D3DXCOLOR& tColor) { m_tBaseColor = tColor; }
	void			Set_ColorVariance(_float fVar) { m_fColorVariance = fVar; }
	void			Set_TextureCount(_uint iCount) { m_iTexCount = iCount; }
	void			Set_Texture();

public:
	static CParticleEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CParticleEffect* Clone();

private:
	vector<Particle>	m_vecParticles;
	_uint				m_iMaxParticles;
	_uint				m_iTexCount;

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
	D3DXCOLOR   m_tBaseColor;			// 기본 색상
	_float      m_fColorVariance;   // 색상 변동폭 (0.0 ~ 1.0)

private:
	virtual void	Free();
};
