#include "pch.h"
#include "CCookingSelectUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CCookingSelectBack.h"
#include "CCookingSelectSlot.h"
#include "CCookingInfoCard.h"
#include "CCookingTargetFood.h"
#include "CCookingUIController.h"
#include "CCookingSelectButton.h"

#include "CFontMgr.h"
#include "CDInputMgr.h"
#include "CPersistentMgr.h"

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

	pGameObject = m_pCookingtargetFood = CCookingTargetFood::Create(
		m_pGraphicDev, { -300.0f,200.0f,0.0001f }, 0.3f, FOODTYPE::FT_COUNT);

	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);

	pGameObject = CCookingSelectSlot::Create(m_pGraphicDev, { -300.0f, 200.0f, 0.4f }, 0.3f);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecCookingSelectUI.push_back(pGameObject);

	CCookingSelectSlot* pCSS;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			pCSS = CCookingSelectSlot::Create(m_pGraphicDev, { -500.f + (75.f * j), -150.f + (75.f * i), 0.001f }, 0.3f);

			if (nullptr == pCSS)
				return E_FAIL;

			//m_vecCookingSelectUI.push_back(pGameObject);
			m_vecFoodSlot.push_back(pCSS);
		}
	}

	CCookingTargetFood* pFood;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			
			pFood = CCookingTargetFood::Create(m_pGraphicDev, { -500.f + (75.f * j), -150.f + (75.f * i), 0.0001f }, 0.3f, FOODTYPE::FT_QUALITY, 3);
			/*CCookingTargetFood* pFood =
				dynamic_cast<CCookingTargetFood*>(pGameObject);*/

			pFood->Set_Render(false);

			if (nullptr == pGameObject)
				return E_FAIL;

			//m_vecCookingSelectUI.push_back(pGameObject);
			m_vecFood.push_back(pFood);
		}
	}

	pGameObject = m_pCookingBtn = CCookingSelectButton::Create(m_pGraphicDev, 1, { -300.0f,-280.0f,0.01f }, 0.5f);
	if (nullptr == pGameObject)
		return E_FAIL;
	m_vecCookingSelectUI.push_back(pGameObject);



	pGameObject = m_pCookingInfoCard = CCookingInfoCard::Create(m_pGraphicDev, m_pMessageChannel);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_iCookingCountMax = 12;
	m_iCurCookingCount = 0;
	m_iResourceCount = 0;
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
		for (CGameObject* Obj : m_vecFoodSlot)
		{
			Obj->Update_GameObject(fTimeDelta);
		}
		for (CGameObject* Obj2 : m_vecFood)
		{
			Obj2->Update_GameObject(fTimeDelta);
		}
		m_pCookingInfoCard->Update_GameObject(fTimeDelta);
	}

	//CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return NOEVENT;
}

void CCookingSelectUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) return;

	if (m_iCurCookingCount >= 1)
	{
		m_pCookingBtn->Changeimage(1);
	}
	else
		m_pCookingBtn->Changeimage(0);


	for (CGameObject* CookingUI : m_vecCookingSelectUI)
	{
		CookingUI->LateUpdate_GameObject(fTimeDelta);
	}
	for (CGameObject* Obj : m_vecFoodSlot)
	{
		Obj->LateUpdate_GameObject(fTimeDelta);
	}
	for (CGameObject* Obj2 : m_vecFood)
	{
		Obj2->LateUpdate_GameObject(fTimeDelta);
	}
	m_pCookingInfoCard->LateUpdate_GameObject(fTimeDelta);
}

void CCookingSelectUI::Render_GameObject()
{

}

void CCookingSelectUI::OnCollision(CGameObject* pObject)
{

}

CCookingSelectUI* CCookingSelectUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CCookingSelectUI* pInstance = new CCookingSelectUI(pGraphicDev);

	pInstance->m_pMessageChannel = pMessageChannel;
	pInstance->m_pMessageChannel->AddRef();

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
	if (m_iCurCookingCount < m_iCookingCountMax && m_pCookingtargetFood->Get_CraftableCount() > 0)
	{
		m_vecFood[m_iCurCookingCount]->Set_Render(true);
		++m_iCurCookingCount;
		m_iResourceCount -= m_pCookingInfoCard->Get_RecipeCount();
		m_pCookingtargetFood->ApplayCraftableCount(-1);
		m_pCookingInfoCard->ApplayRecourceCountText(-m_pCookingInfoCard->Get_RecipeCount());
	}
}

void CCookingSelectUI::DeleteFood()
{
	if (m_iCurCookingCount > 0)
	{
		--m_iCurCookingCount;
		m_vecFood[m_iCurCookingCount]->Set_Render(false);
		m_iResourceCount += m_pCookingInfoCard->Get_RecipeCount();
		m_pCookingInfoCard->ApplayRecourceCountText(m_pCookingInfoCard->Get_RecipeCount());
		m_pCookingtargetFood->ApplayCraftableCount(1);
	}
}

void CCookingSelectUI::ReSetSelecting()
{
	for (CCookingTargetFood* pFood : m_vecFood)
	{
		pFood->Set_Render(false);
	}
	m_iCurCookingCount = 0;
}


void CCookingSelectUI::init_data()
{
	ReSetSelecting();
	m_iResourceCount = CPersistentMgr::GetInstance()->Get_Inventory()->GetItemCount(CItem::IG_BERRY);
	m_pCookingtargetFood->Set_CraftableCount(m_iResourceCount / m_pCookingInfoCard->Get_RecipeCount());
	m_pCookingInfoCard->Init_CalcuResouceCount(m_iResourceCount);
}

_int CCookingSelectUI::Get_RecipeCount()
{
	return m_pCookingInfoCard->Get_RecipeCount();
}

void CCookingSelectUI::Free()
{
	for (CGameObject*& CookingUI : m_vecCookingSelectUI)
	{
		Safe_Release(CookingUI);
	}
	m_vecCookingSelectUI.clear();

	for (auto& ptr : m_vecFoodSlot)
	{
		Safe_Release(ptr);
	}
	m_vecFoodSlot.clear();

	for (auto& ptr : m_vecFood)
	{
		Safe_Release(ptr);
	}
	m_vecFood.clear();
	Safe_Release(m_pCookingInfoCard);

	CUi::Free();
}
