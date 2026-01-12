#include "pch.h"
#include "CCookingInfoCard.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


#include "CCookingInfoCardBack.h"
#include "CCookingTargetFood.h"

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

	pGameObject = CCookingTargetFood::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingSelectUI.push_back(pGameObject);


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
	wchar_t szFoodName[16];
	wchar_t szexexplanation[16];
	wchar_t szEffect[32];

	swprintf_s(szFoodName, L"간장계란밥");
	RECT rc0Player = { 0, 0, 1000, 250 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szFoodName, rc0Player, FontColor, DT_RIGHT | DT_BOTTOM);

	swprintf_s(szexexplanation, L"정말맛있다");
	RECT rc1Player = { 0, 0, 1000,  350 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szexexplanation, rc1Player, FontColor, DT_RIGHT | DT_BOTTOM);

	swprintf_s(szEffect, L"15퍼센트의 확률로\n 추종자가 즉시 똥을 쌉니다");
	RECT rc2Player = { 0, 0, 1100, 400 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szEffect, rc2Player, FontColor, DT_RIGHT | DT_BOTTOM);


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