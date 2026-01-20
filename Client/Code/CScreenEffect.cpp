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
	return S_OK;
}

_int CScreenEffect::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState != ES_PLAY || m_eState != ES_LOOP) return NOEVENT;

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
		if (m_bLoop) m_fAccTime = 0.f;
		else
		{
			m_iHp = 0;
			return NOEVENT;
		}
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return NOEVENT;
}

void CScreenEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
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

CScreenEffect* CScreenEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _uint iTexIdx)
{
	CScreenEffect* pEffect = nullptr;// = new CScreenEffect(pGraphicDev);

	//pEffect->m_iTexIdx = iTexIdx;

	//if (FAILED(pEffect->Ready_GameObject()))
	//{
	//	Safe_Release(pEffect);
	//	MSG_BOX("pEffect Create Failed");
	//	return nullptr;
	//}

	//pEffect->m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z - 0.01f);
	//pEffect->m_pTransformCom->Update_Component(0.f);

	return pEffect;
}

void CScreenEffect::Free()
{
}
