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
	_bool   Is_Exiting() const { return m_bExiting; }

	void    Set_BarHeight(_float fRatio) { m_fBarHeight = fRatio; }
	void    Set_TransitionTime(_float fTime) { m_fTransitionTime = fTime; }
	void    Set_Color(DWORD dwColor) { m_dwColor = dwColor; }

private:
	void    Update_Enter(const _float& fTimeDelta);
	void    Update_Exit(const _float& fTimeDelta);
	void    Render_Bar(_float fCenterY, _float fHeight);
	void    Setup_Orthographic();
	void    Restore_Matrices();

private:
	DWORD   m_dwColor;
	_float  m_fBarHeight;
	_float  m_fCurrentHeight;
	_float  m_fTransitionTime;
	_bool   m_bExiting;
	_matrix m_matOldView;
	_matrix m_matOldProj;

public:
	static	CLetterBox* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey = L"");
	virtual	CLetterBox* Clone() override;

private:
	virtual void Free() override;
};
