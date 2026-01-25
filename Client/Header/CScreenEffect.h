#pragma once
#include "CEffect.h"

class CScreenEffect : public CEffect
{
private:
	explicit	CScreenEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CScreenEffect(const CScreenEffect& rhs);
	virtual		~CScreenEffect();

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

	void			Set_TextureKey(wstring strKey) { m_strProtoTexKey = strKey; }
	void			Set_FadeTime(const _float& fFadeIn, const _float& fFadeOut) { m_fFadeInTime = fFadeIn; m_fFadeOutTime = fFadeOut; }
	void			Pulse(const _float& fTimeDelta);
	void			Rotation(const _float& fTimeDelta);
	void			Zoom(const _float& fTimeDelta);

private:
	_float	m_fAlpha;           // 전체 투명도 (0~1)
	_float	m_fScale;           // 크기 (줌 효과용)
	_float	m_fRotation;        // 회전 (동적 효과용)

	_float	m_fLifeTime;        // 지속 시간
	_float	m_fFadeInTime;      // 페이드인 시간
	_float	m_fFadeOutTime;     // 페이드아웃 시간

	_float m_fPulseTimer;
	_float m_fPulseSpeed;
	_float m_fBaseAlpha;
	_float m_fBaseScale;

public:
	static CScreenEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey);
	virtual CScreenEffect* Clone();

private:
	virtual void	Free();
};
