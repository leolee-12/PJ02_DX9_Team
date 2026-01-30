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
	void			Set_GridXY(const _uint& iGridX, const _uint& iGridY) { m_iGridX = iGridX; m_iGridY = iGridY; }
	void			Set_TextureKey(wstring strKey) { m_strProtoTexKey = strKey; }
	void			Set_Size(const _float& fSizeStart, const _float& fSizeEnd) { m_fSizeStart = fSizeStart; m_fSizeEnd = fSizeEnd; }
	void			Set_Gravity(const _vec3& vGravity) { m_vGravity = vGravity; }
	void			Set_Drag(const _float& fDrag) { m_fDrag = fDrag; }
	void			Set_EmitRange(const _vec3& v1, const _vec3& v2) { m_vEmitMinPos = v1; m_vEmitMaxPos = v2; }
	void			Set_SpeedRange(const _vec3& v1, const _vec3& v2) { m_vMinSpeed = v1; m_vMaxSpeed = v2; }
	void			Set_LifeTime(const _float& fTime) { m_fLifeTime = fTime; }
	void			Set_EmitRate(const _float& fRate) { m_fEmitRate = fRate; }
	void			Set_BaseColor(const D3DXCOLOR& tColor) { m_tBaseColor = tColor; }
	//void			Set_ColorVariance(_float fVar) { m_fColorVariance = fVar; }
	void			Set_Texture(const _uint& iTexIdx);
	void			Set_TextureRange(_uint iMin, _uint iMax) { m_iMinTexIdx = iMin; m_iMaxTexIdx = iMax; }

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
	void			Compute_ParticleBBD(_matrix* pOut, const _vec3& vPos, const _float& fSize);

public:
	static CParticleEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey);
	virtual CParticleEffect* Clone();

private:
	vector<Particle>	m_vecParticles;
	_uint				m_iMaxParticles;
	_matrix				m_matTex;

	// 발생 설정
	_float      m_fEmitRate;		// 초당 발생 수
	_float      m_fEmitAcc;			// 발생 누적
	_vec3       m_vEmitMinPos;		// 발생 위치 범위
	_vec3       m_vEmitMaxPos;
	_vec3       m_vMinSpeed;		// 초기 속도 범위
	_vec3       m_vMaxSpeed;
	_bool		m_bFull = false;

	// 물리
	_vec3       m_vGravity;
	_float      m_fDrag;            // 공기 저항

	// 시각
	_float      m_fSizeStart;
	_float      m_fSizeEnd;
	_float      m_fAlphaStart;
	_float      m_fAlphaEnd;
	D3DXCOLOR   m_tBaseColor;			// 기본 색상
	//_float      m_fColorVariance;   // 색상 변동폭 (0.0 ~ 1.0)

	_uint m_iMinTexIdx;  // 사용할 인덱스 시작 (포함)
	_uint m_iMaxTexIdx;  // 사용할 인덱스 끝 (포함)
	_uint m_iGridX;
	_uint m_iGridY;

	//static constexpr _uint GRID_X = 8;
	//static constexpr _uint GRID_Y = 4;

private:
	virtual void	Free();
};
