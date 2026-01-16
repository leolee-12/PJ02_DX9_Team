#include "pch.h"
#include "CCookingInfoCard.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


#include "CCookingInfoCardBack.h"
#include "CCookingTargetFood.h"
#include "CCookingSelectSlot.h"
#include "CDivider.h"
#include "CCookingStar.h"
#include "CCookingInfoRecipe.h"
#include "CCookingInfoFaith.h"
#include "CCookingUpDownArrow.h"

#include "CFontMgr.h"


CCookingInfoCard::CCookingInfoCard(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_bRender(true)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}
CCookingInfoCard::~CCookingInfoCard()
{

}

HRESULT CCookingInfoCard::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	pGameObject = CCookingInfoCardBack::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);

	pGameObject = CCookingTargetFood::Create(m_pGraphicDev, { 320.0f,150.0f,0.2f }, 0.3f, FOODTYPE::FT_NOMAL);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);

	pGameObject = CCookingSelectSlot::Create(m_pGraphicDev, { 320.0f,150.0f,0.4f }, 0.3f);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);

	pGameObject = CCookingInfoRecipe::Create(m_pGraphicDev, { 300,-150,0.1f }, 0.3f);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);

	pGameObject = CCookingInfoFaith::Create(m_pGraphicDev, { 200,-200,0.1f }, 0.3f);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);

	_vec3 DividerPos = _vec3(410.f, 140.f, 0.1f);
	pGameObject = CDivider::Create(m_pGraphicDev, 1, DividerPos, 0.5f);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);

	_vec3 DividerPos1 = _vec3(230.f, 140.f, 0.1f);
	pGameObject = CDivider::Create(m_pGraphicDev, 0, DividerPos1, 0.5f);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);

	pGameObject = CCookingUpDownArrow::Create(m_pGraphicDev, 0, { 230.0f,-200.0f,0.1f }, 0.9f);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);

	pGameObject = CCookingUpDownArrow::Create(m_pGraphicDev, 1, { 170,-50,0.1f }, 0.9f);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);



	for (int i = 0; i < 3; ++i)
	{
		pGameObject = CCookingStar::Create(m_pGraphicDev, 0, { 290.0f + (i * 30.0f),50.0f,0.1f }, 0.2f);

		if (nullptr == pGameObject)
			return E_FAIL;

		m_vecCookingSelectUI.push_back(pGameObject);
	}

	return S_OK;
}

_int CCookingInfoCard::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	for (CGameObject* CookingUI : m_vecCookingSelectUI)
	{
		CookingUI->Update_GameObject(fTimeDelta);
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return NOEVENT;
}

void CCookingInfoCard::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	for (CGameObject* CookingUI : m_vecCookingSelectUI)
	{
		CookingUI->LateUpdate_GameObject(fTimeDelta);
	}
}

void CCookingInfoCard::Render_GameObject()
{
	if (!m_bRender) { return; }

	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	wchar_t szFoodName[32];

	swprintf_s(szFoodName, L"간장계란밥");
	RECT rc0Player = { 850, 250, 1050, 300 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szFoodName, rc0Player, FontColor, DT_CENTER | DT_TOP);

	swprintf_s(szFoodName, L"정말맛있다");
	RECT rc1Player = { 850, 350, 1050, 400 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szFoodName, rc1Player, FontColor, DT_CENTER | DT_TOP);

	swprintf_s(szFoodName, L"간장밥입니다 계란을곁들인");
	RECT rc2Player = { 700, 400, 1200, 450 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szFoodName, rc2Player, FontColor, DT_CENTER | DT_TOP);

	FontColor = D3DXCOLOR(255.f / 256.f, 0.f / 256.f, 0.f / 256.f, 1.f);
	swprintf_s(szFoodName, L"재료");
	RECT rc3Player = { 850, 450, 1050,  500 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szFoodName, rc3Player, FontColor, DT_CENTER | DT_TOP);


}

void CCookingInfoCard::OnCollision(CGameObject* pObject)
{

}


CCookingInfoCard* CCookingInfoCard::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCookingInfoCard* pCookingMiniGame = new CCookingInfoCard(pGraphicDev);

	if (FAILED(pCookingMiniGame->Ready_GameObject()))
	{
		Safe_Release(pCookingMiniGame);
		MSG_BOX("pCookingSelectUI Create Failed");
		return nullptr;
	}

	return pCookingMiniGame;
}

void CCookingInfoCard::Free()
{
	for (CGameObject* CookingUI : m_vecCookingSelectUI)
	{
		Safe_Release(CookingUI);
	}
	m_vecCookingSelectUI.clear();

	CUi::Free();
}
