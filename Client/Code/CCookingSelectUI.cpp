#include "pch.h"
#include "CCookingSelectUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CCookingSelectBack.h"
#include "CCookingSelectSlot.h"
#include "CCookingInfoCard.h"
#include "CCookingTargetFood.h"

#include "CCookingUIController.h" // [ADD]

#include "CFontMgr.h"
#include "CDInputMgr.h"

CCookingSelectUI::CCookingSelectUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_bRender(true)
	, m_iCurCookingCount(0)
	, m_iCookingCountMax(0)
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

	pGameObject = CCookingTargetFood::Create(
		m_pGraphicDev, { -300.0f,200.0f,0.1f }, 0.3f, FOODTYPE::FT_QUALITY, 3);

	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);


	//pGameObject = CCookingSelectSlot::Create(m_pGraphicDev, { -300.0f, 200.0f, 0.4f }, 0.3f);

	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//m_vecCookingSelectUI.push_back(pGameObject);

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			pGameObject = CCookingSelectSlot::Create(m_pGraphicDev,{ -500.f + (75.f * j), -150.f + (75.f * i), 0.4f },0.3f);

			if (nullptr == pGameObject)
				return E_FAIL;

			m_vecCookingSelectUI.push_back(pGameObject);
			m_vecFoodSlot.push_back(pGameObject);
		}
	}

	pGameObject = CCookingInfoCard::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);

	m_iCookingCountMax = 12;
	m_iCurCookingCount = 0;

	return S_OK;
}

_int CCookingSelectUI::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bRender)
	{
		for (CGameObject* CookingUI : m_vecCookingSelectUI)
		{
			CookingUI->Update_GameObject(fTimeDelta);
		}
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return NOEVENT;
}

void CCookingSelectUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) return;

	for (CGameObject* CookingUI : m_vecCookingSelectUI)
	{
		CookingUI->LateUpdate_GameObject(fTimeDelta);
	}
}

void CCookingSelectUI::Render_GameObject()
{
}

void CCookingSelectUI::OnCollision(CGameObject* pObject)
{
}

CCookingSelectUI* CCookingSelectUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCookingSelectUI* pInstance = new CCookingSelectUI(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCookingSelectUI Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CCookingSelectUI::AddFood()
{
	if (m_iCurCookingCount < m_iCookingCountMax)
		++m_iCurCookingCount;
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
