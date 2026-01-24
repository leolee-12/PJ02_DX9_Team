#include "pch.h"
#include "CNPCCommandUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CCookingCover.h"
#include "CCookingGauge.h"
#include "CCookingGaugeBack.h"
#include "CCookingMarker.h"
#include "CCookingTarget.h"
#include "CCookingTargetBack.h"
#include "CCookingButton.h"

#include "CNPCCommandUIBack.h"
#include "CWorkWoodUI.h"
#include "CWorkRockUI.h"
#include "CRestUI.h"
#include "CLookingEyeUI.h"

#include "CFontMgr.h"
#include "CDInputMgr.h"


CNPCCommandUI::CNPCCommandUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CNPCCommandUI::~CNPCCommandUI()
{
}

HRESULT CNPCCommandUI::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	pGameObject = m_pBackUI = CNPCCommandUIBack::Create(m_pGraphicDev, _vec3(0, 175.0f, 0.1f), 2.0f);
	if (nullptr == pGameObject)
		return E_FAIL;
	pGameObject = m_pWoodUI = CWorkWoodUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3(0, 175.0f + 110.0f, 0.01f), 0.3f);
	if (nullptr == pGameObject)
		return E_FAIL;
	pGameObject = m_pRockUI = CWorkRockUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3(-90.0f, 175.0f - 50.0f, 0.01f), 0.3f);
	if (nullptr == pGameObject)
		return E_FAIL;
	pGameObject = m_pRestUI = CRestUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3(90.0f, 175.0f - 50.0f, 0.01f), 0.3f);
	if (nullptr == pGameObject)
		return E_FAIL;
	pGameObject = m_pLookingEyeUI = CLookingEyeUI::Create(m_pGraphicDev, _vec3(0.0f, 175.0f, 0.01f), 0.3f);
	if (nullptr == pGameObject)
		return E_FAIL;


	// 임시
	m_hmapSubHandles.insert({ L"CommandUI1.Close",m_pMessageChannel->Subscribe(L"CWorkWood.Selected",[this](const IMessageChannel::EVENT& Event)
	{
		m_bRender = false;
	}
) });
	m_hmapSubHandles.insert({ L"CommandUI2.Close",m_pMessageChannel->Subscribe(L"CWorkRock.Selected",[this](const IMessageChannel::EVENT& Event)
{
	m_bRender = false;
}
) });
	m_hmapSubHandles.insert({ L"CommandUI3.Close",m_pMessageChannel->Subscribe(L"CWorkRest.Selected",[this](const IMessageChannel::EVENT& Event)
{
	m_bRender = false;
}
) });
	m_hmapSubHandles.insert({ L"CommandUI.Open",m_pMessageChannel->Subscribe(L"Follower.OpenCommaderUI",[this](const IMessageChannel::EVENT& Event)
{
	m_bRender = true;
}
) });

	return S_OK;
}

_int CNPCCommandUI::Update_GameObject(const _float& fTimeDelta)
{

	if (CDInputMgr::GetInstance()->Key_Down(DIK_X))
	{
		//IMessageChannel::EVENT tEvent;
		//tEvent.strType = L"ResourceHistory.AddItem";
		//tEvent.hmapData[L"ItemID"] = tempcount;
		//m_pMessageChannel->Publish(tEvent);
		m_bRender = !m_bRender;
	}

	if (!m_bRender) { return NOEVENT; }
	m_pBackUI->Update_GameObject(fTimeDelta);
	m_pRockUI->Update_GameObject(fTimeDelta);
	m_pWoodUI->Update_GameObject(fTimeDelta);
	m_pRestUI->Update_GameObject(fTimeDelta);
	m_pLookingEyeUI->Update_GameObject(fTimeDelta);
	return NOEVENT;
}

void CNPCCommandUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	m_pBackUI->LateUpdate_GameObject(fTimeDelta);
	m_pRockUI->LateUpdate_GameObject(fTimeDelta);
	m_pWoodUI->LateUpdate_GameObject(fTimeDelta);
	m_pRestUI->LateUpdate_GameObject(fTimeDelta);
	m_pLookingEyeUI->LateUpdate_GameObject(fTimeDelta);
}

void CNPCCommandUI::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pBackUI->Render_GameObject();
	m_pRockUI->Render_GameObject();
	m_pWoodUI->Render_GameObject();
	m_pRestUI->Render_GameObject();
	m_pLookingEyeUI->Render_GameObject();
}

void CNPCCommandUI::OnCollision(CGameObject* pObject)
{

}

CNPCCommandUI* CNPCCommandUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CNPCCommandUI* pNPCCommandUI = new CNPCCommandUI(pGraphicDev);

	pNPCCommandUI->m_pMessageChannel = pMessageChannel;
	pNPCCommandUI->m_pMessageChannel->AddRef();

	if (FAILED(pNPCCommandUI->Ready_GameObject()))
	{
		Safe_Release(pNPCCommandUI);
		MSG_BOX("pNPCCommandUI Create Failed");
		return nullptr;
	}

	return pNPCCommandUI;
}

void CNPCCommandUI::Free()
{
	Safe_Release(m_pBackUI);
	Safe_Release(m_pWoodUI);
	Safe_Release(m_pRockUI);
	Safe_Release(m_pRestUI);
	Safe_Release(m_pLookingEyeUI);
	CUi::Free();
}
