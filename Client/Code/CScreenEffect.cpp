#include "pch.h"
#include "CScreenEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CScreenEffect::CScreenEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CScreenEffect::CScreenEffect(const CScreenEffect& rhs)
	: CEffect(rhs)
{
}

CScreenEffect::~CScreenEffect()
{
}

HRESULT	CScreenEffect::Ready_GameObject()
{
	FAILED_CHECK_RETURN(CEffect::Add_Component(), E_FAIL);

	m_eType = EF_SCREEN;
	m_eState = ES_READY;
	m_fAlpha = 0.f;
	m_fScale = 1.f;
	m_fRotation = 0.f;

	return S_OK;
}

_int CScreenEffect::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState != ES_PLAY && m_eState != ES_LOOP) return NOEVENT;

	m_fAccTime += fTimeDelta;

	// 알파 계산 (페이드 인/아웃)
	if (m_fAccTime < m_fFadeInTime)
	{
		m_fAlpha = m_fAccTime / m_fFadeInTime;
	}
	else if (m_fAccTime > m_fLifeTime - m_fFadeOutTime)
	{
		_float fRemain = m_fLifeTime - m_fAccTime;
		m_fAlpha = fRemain / m_fFadeOutTime;
	}
	else m_fAlpha = 1.f;

	// 종료
	if (m_fAccTime >= m_fLifeTime)
	{
		if (m_bLoop)
		{
			m_fAccTime = 0.f;
			m_eState = ES_LOOP;
			OnLoop();
		}
		else
		{
			m_eState = ES_FINISH;
			OnFinish();
			return DEAD;
		}
	}

	//Rotation(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CScreenEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CScreenEffect::Render_GameObject()
{
	if (m_fAlpha <= 0.f) return;

	DWORD dwAlpha = DWORD(m_fAlpha * 255.f);
	DWORD dwOldColor, dwOldAlphaOp, dwOldAlphaArg1, dwOldAlphaArg2;
	m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldColor);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwOldAlphaOp);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwOldAlphaArg1);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwOldAlphaArg2);

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwAlpha, 255, 255, 255));
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldColor);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwOldAlphaOp);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwOldAlphaArg1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwOldAlphaArg2);
}

void CScreenEffect::Play()
{
	m_eState = ES_PLAY;
	m_fAccTime = 0.f;
	m_fAlpha = 1.f;
	OnPlay();
}

void CScreenEffect::Stop()
{
}

void CScreenEffect::Reset()
{
}

void CScreenEffect::Pulse(const _float& fTimeDelta)
{
	m_fPulseTimer += m_fPulseSpeed * fTimeDelta;

	_float fPulse = sinf(m_fPulseTimer) * 0.5f + 0.5f;	// 0 ~ 1

	m_fAlpha = m_fBaseAlpha + fPulse * 0.3f;	// 알파 진동
	m_fScale = m_fBaseScale + fPulse * 0.1f;	// 스케일 진동
}

void CScreenEffect::Rotation(const _float& fTimeDelta)
{
	m_fRotation += D3DXToRadian(10.f) * fTimeDelta;

	m_fRotation = sinf(m_fAccTime * 5.f) * D3DXToRadian(3.f);
}

void CScreenEffect::Zoom(const _float& fTimeDelta)
{
	if (m_eType == SEF_DARK)
	{
		_float fRatio = m_fAccTime / m_fLifeTime;
		m_fScale = MyLerp(1.5f, 1.f, fRatio);
	}
	else
	{
		_float fRatio = m_fAccTime / m_fLifeTime;
		m_fScale = MyLerp(0.8f, 1.5f, fRatio);
	}
}

CScreenEffect* CScreenEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScreenEffect* pEffect = nullptr;

	return pEffect;
}

CScreenEffect* CScreenEffect::Clone()
{
	return new CScreenEffect(*this);
}

void CScreenEffect::Free()
{
}
