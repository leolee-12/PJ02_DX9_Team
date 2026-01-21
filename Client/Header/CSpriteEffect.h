#pragma once
#include "CEffect.h"

class CSpriteEffect : public CEffect
{
public:
	typedef struct tagSpriteData
	{
		_uint	iGridX;
		_uint	iGridY;
		_uint	iFrameEnd;
		_float	fFrameSpeed;

		tagSpriteData() : iGridX(1), iGridY(1), iFrameEnd(1), fFrameSpeed(0.f) {}
		tagSpriteData(_uint _iGridX, _uint _iGridY, _uint _iFrameEnd, _float _fFrameSpeed)
			: iGridX(_iGridX), iGridY(_iGridY), iFrameEnd(_iFrameEnd), fFrameSpeed(_fFrameSpeed) {}
	}SPRITE_DATA;

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

	void			Set_SpriteData(const SPRITE_DATA& tData) { m_tSpriteData = tData; }
	void			Set_Billboard(_bool bEnable) { m_bBillboard = bEnable; }
	void			Set_Texture();

	_float		m_fFrame;
	_matrix     m_matTex;

	_float		m_fAlpha;
	_float		m_fAlphaDecay;		// 알파 감소 속도

	SPRITE_DATA m_tSpriteData;
	_float      m_fCurFrame;
	_bool       m_bBillboard;

public:
	static CSpriteEffect*	Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey);
	virtual CSpriteEffect*	Clone();

private:
	virtual void	Free();
};
