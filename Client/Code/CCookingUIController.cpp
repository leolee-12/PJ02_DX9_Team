#include "pch.h"
#include "CCookingUIController.h"

#include "CRenderer.h"
#include "CCookingSelectUI.h"
#include "CCookingMiniGameUI.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"
#include "CPersistentMgr.h"

CCookingUIController::CCookingUIController(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pSelectUI(nullptr)
	, m_pMiniGameUI(nullptr)
	, m_eState(CS_IDLE)
{
}

CCookingUIController::~CCookingUIController()
{

}

HRESULT CCookingUIController::Ready_GameObject()
{
	m_pSelectUI = CCookingSelectUI::Create(m_pGraphicDev, m_pMessageChannel);
	if (nullptr == m_pSelectUI)
		return E_FAIL;

	m_pMiniGameUI = CCookingMiniGameUI::Create(m_pGraphicDev, m_pMessageChannel);
	if (nullptr == m_pMiniGameUI)
		return E_FAIL;

	m_eState = CS_IDLE;
	Ready_Event();

	return S_OK;
}

_int CCookingUIController::Update_GameObject(const _float& fTimeDelta)
{
	Key_Input_Cooking();
	State_Machine();

	m_pSelectUI->Update_GameObject(fTimeDelta);
	m_pMiniGameUI->Update_GameObject(fTimeDelta);



	//CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return NOEVENT;
}

void CCookingUIController::LateUpdate_GameObject(const _float& fTimeDelta)
{
	m_pSelectUI->LateUpdate_GameObject(fTimeDelta);
	m_pMiniGameUI->LateUpdate_GameObject(fTimeDelta);
}

void CCookingUIController::Render_GameObject()
{
}

void CCookingUIController::OnCollision(CGameObject* pObject)
{
}

void CCookingUIController::Start_Cooking(_int iCookingCount)
{
	if (m_pSelectUI->Get_CookingCount() <= 0) { return; }
	m_pMiniGameUI->CookingStart(iCookingCount);
	Set_CookingState(COOKINGUISTATE::CS_MINIGAME);

	CPersistentMgr::GetInstance()->Get_ResourceHistory()->UseItem(TYPE_BERRY, -iCookingCount * m_pSelectUI->Get_RecipeCount());
}

void CCookingUIController::CalcuCookingAbleCount()
{

}

void CCookingUIController::Key_Input_Cooking()
{
	if (m_eState == CS_MINIGAME)
	{
		if (CDInputMgr::GetInstance()->Key_Down(DIK_K))
		{
			m_pMiniGameUI->CookingInput();
		}
		return;
	}

	if (m_eState == CS_SELECT) {
		if (CDInputMgr::GetInstance()->Key_Down(DIK_H))
		{
			m_pSelectUI->AddFood();
		}
		if (CDInputMgr::GetInstance()->Key_Down(DIK_N))
		{
			m_pSelectUI->DeleteFood();
		}
		if (CDInputMgr::GetInstance()->Key_Down(DIK_J))
		{
			Start_Cooking(m_pSelectUI->Get_CookingCount());
		}
	}
}

void CCookingUIController::State_Machine()
{
	if (m_ePrevState != m_eState)
	{
		switch (m_eState)
		{
		case CS_IDLE:
			m_pSelectUI->SetRender(false);
			m_pMiniGameUI->Set_Render(false);
			break;
		case CS_SELECT:
			m_pSelectUI->init_data();
			m_pSelectUI->SetRender(true);
			m_pMiniGameUI->Set_Render(false);
			break;

		case CS_MINIGAME:
			m_pSelectUI->ReSetSelecting();
			m_pSelectUI->SetRender(false);
			m_pMiniGameUI->Set_Render(true);
			break;
		case CS_COOKINGEND:
			m_pSelectUI->SetRender(false);
			m_pMiniGameUI->Set_Render(false);
			m_pSelectUI->Set_CookingCount(0);
			break;
		}
		m_ePrevState = m_eState;
	}
}

void CCookingUIController::Ready_Event()
{
	//m_pMiniGameUI->Set_CookingEndCallback([this]()
	//	{
	//		Set_CookingState(COOKINGUISTATE::CS_SELECT);
	//	});

	m_hmapSubHandles.insert({ L"CookingUI.Finish", m_pMessageChannel->Subscribe(L"Cooking.Finish", [this](const IMessageChannel::EVENT& Event) {
{
		Set_CookingState(COOKINGUISTATE::CS_SELECT);
}
}) });
}

CCookingUIController* CCookingUIController::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CCookingUIController* pInstance = new CCookingUIController(pGraphicDev);

	pInstance->m_pMessageChannel = pMessageChannel;
	pInstance->m_pMessageChannel->AddRef();

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCookingUIController Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CCookingUIController::Free()
{
	Safe_Release(m_pSelectUI);
	Safe_Release(m_pMiniGameUI);

	CUi::Free();
}

