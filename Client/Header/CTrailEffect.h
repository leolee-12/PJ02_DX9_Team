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

public:
	static CTrailEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _uint iTexIdx);

	deque<_vec3>    m_dequePoints;      // 궤적 점들
	_uint           m_iMaxPoints;
	_float          m_fPointInterval;   // 점 추가 간격 (거리)
	_vec3           m_vLastPoint;

	_float          m_fWidth;           // 트레일 너비
	_float          m_fAlphaStart;
	_float          m_fAlphaEnd;
	DWORD           m_dwColor;

private:
	virtual void	Free();
};
