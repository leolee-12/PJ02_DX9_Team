#include "pch.h"
#include "CCookingMiniGameUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CCookingCover.h"
#include "CCookingGauge.h"
#include "CCookingGaugeBack.h"
#include "CCookingMarker.h"
#include "CCookingTarget.h"
#include "CCookingTargetBack.h"
#include "CCookingButton.h"


#include "CFontMgr.h"


CCookingMiniGameUI::CCookingMiniGameUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_bRender(true)
	, m_pMarker(nullptr)
	, m_pGauge(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CCookingMiniGameUI::~CCookingMiniGameUI()
{
}

HRESULT CCookingMiniGameUI::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	pGameObject = CCookingCover::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingUI.push_back(pGameObject);


	pGameObject = m_pGauge = CCookingGauge::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingUI.push_back(pGameObject);


	pGameObject = CCookingGaugeBack::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingUI.push_back(pGameObject);


	pGameObject = m_pMarker = CCookingMarker::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingUI.push_back(pGameObject);




	pGameObject = CCookingTarget::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingUI.push_back(pGameObject);

	pGameObject = CCookingTargetBack::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingUI.push_back(pGameObject);

	pGameObject = CCookingButton::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingUI.push_back(pGameObject);


	m_bRender = false;
	m_iCurCookingCount = 0;
	m_iCookingCount = 0;
	return S_OK;
}

_int CCookingMiniGameUI::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	for (CGameObject* CookingUI : m_vecCookingUI)
	{
		CookingUI->Update_GameObject(fTimeDelta);
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return NOEVENT;
}

void CCookingMiniGameUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	for (CGameObject*  CookingUI: m_vecCookingUI)
	{
		CookingUI->LateUpdate_GameObject(fTimeDelta);
	}
}

void CCookingMiniGameUI::Render_GameObject()
{
	//if (!m_bPontRender) { return; }
	if (!m_bRender) { return; }
	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	wchar_t szCookingCount[16];

	swprintf_s(szCookingCount, L"%d/%d", m_iCurCookingCount, m_iCookingCount);
	RECT rcPlayer = { 0, 0, 700, 130 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szCookingCount, rcPlayer, FontColor, DT_RIGHT | DT_BOTTOM);

	// 진짜 임시테스트용 멈추는지, 숫자 카운팅되는지, 랜덤하게 이동하는지확인용
}

void CCookingMiniGameUI::OnCollision(CGameObject* pObject)
{

}

_bool CCookingMiniGameUI::Check_CookingResult()
{
	float fMarkerX = m_pMarker->Get_MarkerPos();

	float fGaugeLeft = 0.f;
	float fGaugeRight = 0.f;
	m_pGauge->Get_GaugeRange(fGaugeLeft, fGaugeRight);

	if (fMarkerX >= fGaugeLeft && fMarkerX <= fGaugeRight)
		return true;

	return false;
}

_bool CCookingMiniGameUI::CookingInput()
{

	m_pMarker->Stop_Marker();
	_bool bResult = Check_CookingResult();
	m_pGauge->Set_RandomPosX();

	if (bResult)
		m_iCurCookingCount--;

	return bResult;
}


CCookingMiniGameUI* CCookingMiniGameUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCookingMiniGameUI* pCookingMiniGame = new CCookingMiniGameUI(pGraphicDev);

	if (FAILED(pCookingMiniGame->Ready_GameObject()))
	{
		Safe_Release(pCookingMiniGame);
		MSG_BOX("pCookingMiniGameUI Create Failed");
		return nullptr;
	}

	return pCookingMiniGame;
}

void CCookingMiniGameUI::SetRenderChange()
{
	if (m_bRender == false)
	{
		m_bRender = true;
	}
	else
	{
		m_bRender = false;
	}
}

void CCookingMiniGameUI::Free()
{
	for (CGameObject* CookingUI : m_vecCookingUI)
	{
		Safe_Release(CookingUI);
	}
	m_vecCookingUI.clear();

	CUi::Free();
}