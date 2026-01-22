#pragma once
#include "CEffect.h"

class CTrailEffect : public CEffect
{
private:
	explicit	CTrailEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CTrailEffect(const CTrailEffect& rhs);
	virtual		~CTrailEffect();

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

	void			Add_Point(const _vec3& vPoint);
	void			Compute_TrailWorldMatrix();

private:
	deque<_vec3>	m_dequePoints;      // 궤적 점들
	_uint			m_iMaxPoints;
	_float			m_fPointInterval;   // 점 추가 간격 (거리)
	_vec3			m_vLastPoint;

	_float			m_fWidth;           // 트레일 너비
	_float			m_fAlphaStart;
	_float			m_fAlphaEnd;
	DWORD			m_dwColor;

	// 위치/방향
	_vec3       m_vPos;
	_vec3       m_vSpeed;

	// 크기
	_float      m_fHeadSize;        // 머리 크기 (반지름)
	_float      m_fTailLength;      // 꼬리 길이
	_bool       m_bScaleBySpeed;    // 속도 비례 길이 여부

	// 색상
	D3DXCOLOR   m_tColor;
	_bool       m_bEmissive;	// Emissive 사용 여부

	_matrix		m_matTrailWorld;

public:
	static		CTrailEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey);
	virtual		CTrailEffect* Clone();

private:
	virtual void	Free();
};
