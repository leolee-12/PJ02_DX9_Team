#pragma once
#include "CEffect.h"

class CChargeArrow : public CEffect
{
private:
	explicit	CChargeArrow(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CChargeArrow(const CChargeArrow& rhs);
	virtual		~CChargeArrow();

public:
	virtual HRESULT	Ready_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject();

	// 필수
	virtual void    Play() override;
	virtual void    Stop() override;
	virtual void    Reset() override;

	void			Update_OwnerData(const _vec3& vPos, const _vec3& vDir);

	// ─── 런타임 값 ───
	_vec3       m_vDir;				// 계산된 착탄 지점
	_matrix		m_matWorld;			// 월드 행렬

public:
	static	CChargeArrow* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey);
	virtual CChargeArrow* Clone();

private:
	virtual void	Free();
};
