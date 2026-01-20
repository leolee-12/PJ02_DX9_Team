#pragma once
#include "CEffect.h"

class CSpriteEffect : public CEffect
{
private:
	explicit	CSpriteEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CSpriteEffect(const CSpriteEffect& rhs);
	virtual		~CSpriteEffect();

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
	static CSpriteEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _uint iTexIdx);

	_float		m_fFrame;
	_float		m_fFrameEnd;
	_float		m_fFrameSpeed;

	_vec3		m_vScale;
	_float		m_fAlpha;
	_float		m_fAlphaDecay;		// 알파 감소 속도

private:
	virtual void	Free();
};
