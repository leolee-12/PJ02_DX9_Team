#include "pch.h"
#include "CCookingSelectUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CCookingSelectBack.h"
#include "CCookingSelectSlot.h"
#include "CCookingInfoCard.h"
#include "CCookingTargetFood.h"

#include "CCookingMiniGameUI.h"

#include "CFontMgr.h"


CCookingSelectUI::CCookingSelectUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_bRender(true)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CCookingSelectUI::~CCookingSelectUI()
{
}

HRESULT CCookingSelectUI::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	pGameObject = CCookingSelectBack::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingSelectUI.push_back(pGameObject);


	pGameObject = m_pCookingMiniGameUI = CCookingMiniGameUI::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingSelectUI.push_back(pGameObject);


	//pGameObject = CCookingSelectSlot::Create(m_pGraphicDev, { -300,200,0.4f }, 0.4f);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//m_vecCookingSelectUI.push_back(pGameObject);

	pGameObject = CCookingTargetFood::Create(m_pGraphicDev,{-300,200,0.1f},0.4f, FOODTYPE::FT_QUALITY,3);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingSelectUI.push_back(pGameObject);


	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			pGameObject = CCookingSelectSlot::Create(m_pGraphicDev, { -700.0f+(100.0f*j),-100.f +(100.0f*i) ,0.4f}, 0.4f);

			if (nullptr == pGameObject)
				return E_FAIL;

			m_vecCookingSelectUI.push_back(pGameObject);
			m_vecFoodSlot.push_back(pGameObject);
		}
	}

	pGameObject = CCookingInfoCard::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;
	m_iCookingCountMax = 12;
	m_iCurCookingCount = 5;
	m_vecCookingSelectUI.push_back(pGameObject);

	return S_OK;
}

_int CCookingSelectUI::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	for (CGameObject* CookingUI : m_vecCookingSelectUI)
	{
		CookingUI->Update_GameObject(fTimeDelta);
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return NOEVENT;
}

void CCookingSelectUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	for (CGameObject* CookingUI : m_vecCookingSelectUI)
	{
		CookingUI->LateUpdate_GameObject(fTimeDelta);
	}
}

void CCookingSelectUI::Render_GameObject()
{
	if (!m_bRender) { return; }

}

void CCookingSelectUI::OnCollision(CGameObject* pObject)
{

}


CCookingSelectUI* CCookingSelectUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCookingSelectUI* pCookingMiniGame = new CCookingSelectUI(pGraphicDev);

	if (FAILED(pCookingMiniGame->Ready_GameObject()))
	{
		Safe_Release(pCookingMiniGame);
		MSG_BOX("pCookingSelectUI Create Failed");
		return nullptr;
	}

	return pCookingMiniGame;
}

void CCookingSelectUI::AddFood()
{
	m_iCurCookingCount++;
}

void CCookingSelectUI::MakeFood()
{
	//SetRender(false);
	m_pCookingMiniGameUI->Set_Render(true);
	m_pCookingMiniGameUI->CookingStart(m_iCurCookingCount);
}

void CCookingSelectUI::Free()
{
	for (CGameObject* CookingUI : m_vecCookingSelectUI)
	{
		Safe_Release(CookingUI);
	}
	m_vecCookingSelectUI.clear();

	CUi::Free();
}