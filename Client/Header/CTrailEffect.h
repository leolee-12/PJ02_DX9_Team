#pragma once
#include "CEffect.h"

class CTrailEffect : public CEffect
{
private:
	explicit	CTrailEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CTrailEffect(const CTrailEffect& rhs);
	virtual		~CTrailEffect();

public:
	void			Set_Color(const D3DXCOLOR& tColor) { m_tColor = tColor; }
	void			Set_HeadSize(_float fHeadSize) { m_fHeadSize = fHeadSize;}
	void			Set_TailLength(_float fTailLength) { m_fTailLength = fTailLength; }
	void			Set_Emissive(_bool bEmissive) { m_bEmissive = bEmissive; }
	void			Update_OwnerData(const _vec3& vPos, const _vec3& vSpeed) { m_vPos = vPos; m_vSpeed = vSpeed; }

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

	void			Compute_TrailWorldMatrix();

private:
	// 위치/방향
	_vec3       m_vPos;
	_vec3       m_vSpeed;
	_matrix		m_matTrailWorld;

	// 크기
	_float      m_fHeadSize;        // 머리 크기 (반지름)
	_float      m_fTailLength;      // 꼬리 길이
	_bool       m_bScaleBySpeed;    // 속도 비례 길이 여부

	// 색상
	D3DXCOLOR   m_tColor;
	_bool       m_bEmissive;	// Emissive 사용 여부


public:
	static		CTrailEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey);
	virtual		CTrailEffect* Clone();

private:
	virtual void	Free();
};
