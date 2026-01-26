#include "pch.h"
#include "CFoodReviewUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CFoodReviewBack.h"
#include "CFoodReviewImage.h"
#include "CFontUIOrtho.h"

#include "CFontMgr.h"
#include "CDInputMgr.h"
#include "CSoundMgr.h"

CFoodReviewUI::CFoodReviewUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CFoodReviewUI::~CFoodReviewUI()
{
}

HRESULT CFoodReviewUI::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	pGameObject = m_pReviewBackUI = CFoodReviewBack::Create(m_pGraphicDev, _vec3(0, 200.0f, 0.1f), 0.5f);
	if (nullptr == pGameObject)
		return E_FAIL;
	pGameObject = m_pReviewImageUI = CFoodReviewImage::Create(m_pGraphicDev, _vec3(0, 300.0f, 0.01f), 0.5f, 0);
	if (nullptr == pGameObject)
		return E_FAIL;

	pGameObject = m_pAHNReivewFont = CFontUIOrtho::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pAHNReivewFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pAHNReivewFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pAHNReivewFont->Set_Pos(_vec2(65.0f, 168.0f));
	m_pAHNReivewFont->Set_Scale(_vec2(59.f * 2.f, 123.f * 0.5f));
	m_pAHNReivewFont->Set_Font(L"Font_Default30_Heavy");
	m_pAHNReivewFont->Active();
	m_pAHNReivewFont->Set_Text((L"생존"));

	pGameObject = m_pBackReivewFont = CFontUIOrtho::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pBackReivewFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pBackReivewFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pBackReivewFont->Set_Pos(_vec2(-65.0f, 168.0f));
	m_pBackReivewFont->Set_Scale(_vec2(59.f * 2.f, 123.f * 0.5f));
	m_pBackReivewFont->Set_Font(L"Font_Default30_Heavy");
	m_pBackReivewFont->Active();
	m_pBackReivewFont->Set_Text((L"생존"));
	m_bActive = false;
	Ready_Event();
	return S_OK;
}

void CFoodReviewUI::Ready_Event()
{
	m_hmapSubHandles.insert({ L"FoodReview.Open",m_pMessageChannel->Subscribe(L"Trigger.Activate.Owner" ,[this](const IMessageChannel::EVENT& Event)
{
		auto iter = Event.hmapData.find(L"Trigger_Name");
		if (iter == Event.hmapData.end())
			return;

		wstring name = any_cast<wstring>(iter->second);
		if (name == L"GoodFood")
		{
			ReviewGoodFood();
		}
		else if (name == L"BadFood")
		{
			ReviewBadFood();
		}
}
) });

}
_int CFoodReviewUI::Update_GameObject(const _float& fTimeDelta)
{
	m_fRenderTimer += fTimeDelta;

	//if (CDInputMgr::GetInstance()->Key_Down(DIK_C))
	//{
	//
	//	ReviewGoodFood();
	//}
	//if (CDInputMgr::GetInstance()->Key_Down(DIK_V))
	//{
	//	ReviewBadFood();
	//}

	if (!m_bActive) { return NOEVENT; }
	m_pReviewBackUI->Update_GameObject(fTimeDelta);

	if (m_fRenderTimer >= 0.5f)
	{
		m_pBackReivewFont->Update_GameObject(fTimeDelta);
	}
	if (m_fRenderTimer >= 1.0f)
	{
		m_pAHNReivewFont->Update_GameObject(fTimeDelta);
	}
	if (m_fRenderTimer >= 1.5f)
	{
		if (!m_soundPlayed)
		{
			CSoundMgr::GetInstance()->Play(m_szSoundText.c_str(), SOUND_DIALOGUE, 0.6f);
			m_soundPlayed = true;
		}
		m_pReviewImageUI->Update_GameObject(fTimeDelta);
	}
	if (m_fRenderTimer >= 3.0f)
	{
		m_bActive = false;
	}
	return NOEVENT;
}

void CFoodReviewUI::LateUpdate_GameObject(const _float& fTimeDelta)
{

	if (!m_bActive) { return; }
	m_pReviewBackUI->LateUpdate_GameObject(fTimeDelta);
	if (m_fRenderTimer >= 1.0f)
	{
		m_pBackReivewFont->LateUpdate_GameObject(fTimeDelta);
	}
	if (m_fRenderTimer >= 2.0f)
	{
		m_pAHNReivewFont->LateUpdate_GameObject(fTimeDelta);
	}
	if (m_fRenderTimer >= 3.0f)
	{
		m_pReviewImageUI->LateUpdate_GameObject(fTimeDelta);
	}
}

void CFoodReviewUI::Render_GameObject()
{

	if (!m_bActive) { return; }
	m_pReviewBackUI->Render_GameObject();
	if (m_fRenderTimer >= 1.0f)
	{
		m_pBackReivewFont->Render_GameObject();
	}
	if (m_fRenderTimer >= 2.0f)
	{
		m_pAHNReivewFont->Render_GameObject();
	}
	if (m_fRenderTimer >= 3.0f)
	{
	m_pReviewImageUI->Render_GameObject();
	}
}

void CFoodReviewUI::OnCollision(CGameObject* pObject)
{
}

CFoodReviewUI* CFoodReviewUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CFoodReviewUI* pCFoodReviewUI = new CFoodReviewUI(pGraphicDev);

	pCFoodReviewUI->m_pMessageChannel = pMessageChannel;
	pCFoodReviewUI->m_pMessageChannel->AddRef();

	if (FAILED(pCFoodReviewUI->Ready_GameObject()))
	{
		Safe_Release(pCFoodReviewUI);
		MSG_BOX("pFoodReviewUI Create Failed");
		return nullptr;
	}

	return pCFoodReviewUI;
}

void CFoodReviewUI::Free()
{
	Safe_Release(m_pReviewBackUI);
	Safe_Release(m_pReviewImageUI);
	Safe_Release(m_pAHNReivewFont);
	Safe_Release(m_pBackReivewFont);
	CUi::Free();
}

void CFoodReviewUI::ReviewGoodFood()
{
	m_bActive = true;
	m_soundPlayed = false;
	m_fRenderTimer = 0.0f;
	m_pReviewImageUI->Set_Page(1);
	m_pAHNReivewFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pBackReivewFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pAHNReivewFont->Set_Text((L"생존"));
	m_pBackReivewFont->Set_Text((L"생존"));
	m_szSoundText = L"GoodFood.wav";
}

void CFoodReviewUI::ReviewBadFood()
{
	m_bActive = true;
	m_soundPlayed = false;
	m_fRenderTimer = 0.0f;
	m_pReviewImageUI->Set_Page(0);
	m_pAHNReivewFont->Set_FontColor(D3DXCOLOR(1.0f, 0.01f, 0.01f, 1.f));
	m_pBackReivewFont->Set_FontColor(D3DXCOLOR(1.0f, 0.01f, 0.01f, 1.f));
	m_pAHNReivewFont->Set_Text((L"탈락"));
	m_pBackReivewFont->Set_Text((L"탈락"));
	m_szSoundText = L"BadFood.wav";
}

