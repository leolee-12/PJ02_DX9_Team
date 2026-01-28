#pragma once
#include "CEffect.h"

class CLetterBox : public CEffect
{
private:
	explicit CLetterBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CLetterBox(const CLetterBox& rhs);
	virtual ~CLetterBox();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int    Update_GameObject(const _float& fTimeDelta) override;
	virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void    Render_GameObject() override;

	virtual void    Play() override;
	virtual void    Stop() override;
	virtual void    Reset() override;

	void    Exit();     // 퇴장 시작

	void    Set_Color(D3DXCOLOR tColor)			{ m_tColor = tColor; }
	void    Set_BarHeight(_float fRatio)		{ m_fBarHeight = fRatio; }
	void    Set_TransitionTime(_float fTime)	{ m_fTransitionTime = fTime; }

private:
	void    Update_Enter(const _float& fTimeDelta);
	void    Update_Exit(const _float& fTimeDelta);
	void    Render_Bar(_float fCenterY, _float fHeight);

private:
	D3DXCOLOR   m_tColor;
	_float		m_fBarHeight;
	_float		m_fCurrentHeight;
	_float		m_fTransitionTime;
	_bool		m_bExiting;

public:
	static	CLetterBox* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey = L"");
	virtual	CLetterBox* Clone() override;

private:
	virtual void Free() override;
};
