#include "pch.h"
#include "CLetterBox.h"
#include "CRenderer.h"
#include "CRcTex.h"
#include "CTransform.h"

CLetterBox::CLetterBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CLetterBox::CLetterBox(const CLetterBox& rhs)
	: CEffect(rhs)
{
}

CLetterBox::~CLetterBox()
{
}

HRESULT CLetterBox::Ready_GameObject()
{
	FAILED_CHECK_RETURN(CEffect::Add_Component(), E_FAIL);

	m_eType = EF_SCREEN;
	m_eState = ES_READY;

	m_tColor = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	m_fBarHeight = (_float)WINCY * 0.12f;   // 화면의 12%
	m_fCurrentHeight = 0.f;
	m_fTransitionTime = 0.4f;
	m_bExiting = false;

	return S_OK;
}

_int CLetterBox::Update_GameObject(const _float& fTimeDelta)
{
	if (ES_READY == m_eState) return NOEVENT;

	m_fAccTime += fTimeDelta;

	if		(ES_PLAY == m_eState)				{ Update_Enter(fTimeDelta); }
	else if (ES_LOOP == m_eState && m_bExiting) { Update_Exit(fTimeDelta);	}

	if (m_fCurrentHeight > 0.f)
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return NOEVENT;
}

void CLetterBox::LateUpdate_GameObject(const _float& fTimeDelta)
{
	//m_fDepth = 0.1f;
}

void CLetterBox::Render_GameObject()
{
	if (m_fCurrentHeight <= 0.f) return;

	// 텍스처 없이 단색 렌더링
	m_pGraphicDev->SetTexture(0, nullptr);

	DWORD dwOldColor, dwOldColorOp, dwOldColorArg1, dwOldAlphaOp, dwOldAlphaArg1;
	m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldColor);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwOldColorOp);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwOldColorArg1);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwOldAlphaOp);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwOldAlphaArg1);

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_tColor);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

	_float fHalfY = _float(WINCY) * 0.5f;
	_float fHeight = fHalfY - m_fCurrentHeight * 0.5f;

	// 상단 띠
	Render_Bar(fHeight, m_fCurrentHeight);
	// 하단 띠
	Render_Bar(-fHeight, m_fCurrentHeight);

	// 렌더 상태 복원
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldColor);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwOldColorOp);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwOldColorArg1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwOldAlphaOp);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwOldAlphaArg1);
}

void CLetterBox::Play()
{
	m_eState = ES_PLAY;
	m_fAccTime = 0.f;
	m_bExiting = false;
	//OnPlay();
}

void CLetterBox::Stop()
{
	m_eState = ES_READY;
	m_fCurrentHeight = 0.f;
	m_bExiting = false;
}

void CLetterBox::Reset()
{
	Stop();
	m_fAccTime = 0.f;
}

void CLetterBox::Exit()
{
	if (ES_LOOP != m_eState) return;
	m_bExiting = true;
	m_fAccTime = 0.f;
}

void CLetterBox::Update_Enter(const _float& fTimeDelta)
{
	_float fRatio = min(m_fAccTime / m_fTransitionTime, 1.f);
	fRatio = 1.f - (1.f - fRatio) * (1.f - fRatio);  // EaseOut-Quad : f(t) = 1 - (1-t)^2 (빠르게 시작, 천천히 정지, 2차함수)

	m_fCurrentHeight = m_fBarHeight * fRatio;

	if (fRatio >= 1.f)
	{
		m_fCurrentHeight = m_fBarHeight;
		m_eState = ES_LOOP;
		m_fAccTime = 0.f;
	}
}

void CLetterBox::Update_Exit(const _float& fTimeDelta)
{
	_float fRatio = min(m_fAccTime / m_fTransitionTime, 1.f);
	fRatio = fRatio * fRatio;  // EaseIn-Quad : f(t) = t^2 (천천히 시작, 빠르게 정지, 2차함수)

	m_fCurrentHeight = m_fBarHeight * (1.f - fRatio);

	if (fRatio >= 1.f)
	{
		m_fCurrentHeight = 0.f;
		m_eState = ES_READY;
		m_bExiting = false;
		//OnFinish();
	}
}

void CLetterBox::Render_Bar(_float fCenterY, _float fHeight)
{
	_matrix matWorld;
	D3DXMatrixIdentity(&matWorld);

	matWorld._11 = _float(WINCX);			// 가로 = 화면 너비
	matWorld._22 = fHeight * _float(WINCY);	// 세로 = 띠 높이
	matWorld._41 = 0.f;						// X 중앙
	matWorld._42 = fCenterY;				// Y 위치

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	m_pBufferCom->Render_Buffer();
}

CLetterBox* CLetterBox::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey)
{
	CLetterBox* pLetterBox = new CLetterBox(pGraphicDev);

	pLetterBox->m_strProtoTexKey = strProtoTexKey;

	if (FAILED(pLetterBox->Ready_GameObject()))
	{
		Safe_Release(pLetterBox);
		MSG_BOX("pLetterBox Create Failed");
		return nullptr;
	}

	return pLetterBox;
}

CLetterBox* CLetterBox::Clone()
{
	//CLetterBox* pLetterBox = new CLetterBox(*this);
	//
	//if (FAILED(pLetterBox->Add_Component()))
	//{
	//	Safe_Release(pLetterBox);
	//	MSG_BOX("pLetterBox Clone Failed");
	//	return nullptr;
	//}

	// LetterBox는 복사하여 사용 X -> CEffectMgr::Get_LetterBox()로 원본을 직접 사용
	return nullptr;
}

void CLetterBox::Free()
{
	CEffect::Free();
}
