#pragma once
#include "CEffect.h"

class CIndicator : public CEffect
{
private:
	explicit	CIndicator(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CIndicator(const CIndicator& rhs);
	virtual		~CIndicator();

public:
	virtual HRESULT	Ready_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject();

	// 필수
	virtual void    Play() override;
	virtual void    Stop() override;
	virtual void    Reset() override;

	void			Update_OwnerData(const _vec3& vPos, const _vec3& vSpeed, const _float& fGravity, const _float& fGroundY);

private:
	_vec3			Compute_LandingPos(const _vec3& vPos, const _vec3& vSpeed, const _float& fGravity, const _float& fGroundY);

	// ─── 런타임 값 ───
	_vec3       m_vLandingPos;      // 계산된 착탄 지점
	_matrix		m_matWorld;         // 월드 행렬

public:
	static CIndicator* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey);
	virtual CIndicator* Clone();

private:
	virtual void	Free();
};
