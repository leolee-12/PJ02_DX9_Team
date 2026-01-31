#include "pch.h"
#include "CGauge.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"

#include "CGaugeCover.h"
#include "CGaugeStar.h"
#include "CPassionGauge.h"
#include "CPassionIcon.h"

#include "CFontMgr.h"


CGauge::CGauge(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_fGaugeValue(0.f)
	, m_pPassionGauge(nullptr)
	, m_pPassionIcon(nullptr)
	, m_eCurGaugeState(Gauge::GS_END)
	, m_ePreGaugeState(Gauge::GS_END)
	, m_bFontRender(true)
	, m_fMaxPassionGaugeValue(CPlayer::PLAYER_PASSION_MAX)
	, m_fMaxFaithGaugeValue(CPlayer::PLAYER_FAITH_MAX)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CGauge::~CGauge()
{
}

HRESULT CGauge::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	pGameObject = CGaugeCover::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecGaugeUI.push_back(pGameObject);


	pGameObject = m_pGaugeStar = CGaugeStar::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecGaugeUI.push_back(pGameObject);

	pGameObject = m_pPassionGauge = CPassionGauge::Create(m_pGraphicDev, m_eCurGaugeState, m_fMaxPassionGaugeValue, m_fMaxFaithGaugeValue);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecGaugeUI.push_back(pGameObject);


	pGameObject = m_pPassionIcon = CPassionIcon::Create(m_pGraphicDev, m_eCurGaugeState);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecGaugeUI.push_back(pGameObject);

	m_fDepth = 0.1f;

	return S_OK;
}

_int CGauge::Update_GameObject(const _float& fTimeDelta)
{
	Update_State();

	m_pPassionGauge->Set_GaugeValue(m_fGaugeValue);

	for (CGameObject* GaugeUI : m_vecGaugeUI)
	{
		GaugeUI->Update_GameObject(fTimeDelta);
	}

	if (m_eCurGaugeState == Gauge::GS_PASSION) {
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	return NOEVENT;
}

void CGauge::LateUpdate_GameObject(const _float& fTimeDelta)
{
	for (CGameObject* GaugeUI : m_vecGaugeUI)
	{
		GaugeUI->LateUpdate_GameObject(fTimeDelta);
	}

	// test
	Accumulate_GaugeValue(fTimeDelta);
}

void CGauge::Render_GameObject()
{
	//if (!m_bFontRender) { return; }

	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	wchar_t szGauge[16];

	_int iGaugeValue = _int(floor(m_fGaugeValue));

	swprintf_s(szGauge, L"%d", iGaugeValue);
	RECT rcPlayer = { 0, 0, 74, 89 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szGauge, rcPlayer, FontColor, DT_RIGHT | DT_BOTTOM);
}

void CGauge::OnCollision(CGameObject* pObject)
{

}

void CGauge::Update_State()
{
	if (m_eCurGaugeState != m_ePreGaugeState)
	{
		switch (m_eCurGaugeState)
		{
		case Gauge::GS_PASSION:
			m_bFontRender = true;
			m_pPassionGauge->Set_PassionMax(m_fMaxPassionGaugeValue);
			m_pGaugeStar->Set_Render(true);
			break;
		case Gauge::GS_FAITH:
			m_bFontRender = false;
			m_pPassionGauge->Set_FaithMax(m_fMaxFaithGaugeValue);
			m_pGaugeStar->Set_Render(false);
			break;
		}

		m_pPassionGauge->Set_GaugeState(m_eCurGaugeState);
		m_pPassionIcon->Set_GaugeState(m_eCurGaugeState);
		m_ePreGaugeState = m_eCurGaugeState;
	}
}

void CGauge::Accumulate_GaugeValue(const _float& fTimeDelta)
{
	switch (m_eCurGaugeState)
	{
		case Gauge::GS_PASSION:
		{
			if (m_fGaugeValue > m_fMaxPassionGaugeValue)
			{
				m_fGaugeValue = m_fMaxPassionGaugeValue;
				return;
			}

			m_fGaugeValue = CPersistentMgr::GetInstance()->Get_Player()->Get_Passion();
		}
		break;
		case Gauge::GS_FAITH:
		{
			if (m_fGaugeValue > m_fMaxFaithGaugeValue)
			{
				m_fGaugeValue = m_fMaxFaithGaugeValue;
				return;
			}

			m_fGaugeValue = CPersistentMgr::GetInstance()->Get_Player()->Get_Faith();
		}
	}
}



void CGauge::Set_FontRender(_bool bBool)
{
	switch (m_eCurGaugeState)
	{
	case Gauge::GS_PASSION:
		m_bFontRender = bBool;
		break;
	case Gauge::GS_FAITH:
		m_bFontRender = false;
		break;
	}
}

CGauge* CGauge::Create(LPDIRECT3DDEVICE9 pGraphicDev, Gauge::GAUGESTATE eState)
{
	CGauge* pGauge = new CGauge(pGraphicDev);

	pGauge->m_eCurGaugeState = eState;

	if (FAILED(pGauge->Ready_GameObject()))
	{
		Safe_Release(pGauge);
		MSG_BOX("pGauge Create Failed");
		return nullptr;
	}

	return pGauge;
}

void CGauge::Free()
{
	for (CGameObject* GaugeUI : m_vecGaugeUI)
	{
		Safe_Release(GaugeUI);
	}
	m_vecGaugeUI.clear();

	CUi::Free();
}
