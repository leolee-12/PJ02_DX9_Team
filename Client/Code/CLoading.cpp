#include "pch.h"
#include "CLoadingThread.h"
#include "CLoading.h"
#include "CProtoMgr.h"
#include "CStage.h"
#include "CManagement.h"
#include "CFontMgr.h"
#include "CTest.h"
#include "CLoadingBack.h"
#include "CLoadingCenter.h"
#include "CLoadingFG.h"
#include "CLoadingCircle.h"
#include "CLoadingLogo.h"



CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID)
	: CScene(pGraphicDev), m_pLoading(nullptr), m_eChangeID(ChangeID), m_fLoadingDelay(0.f)
{
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Scene()
{
	//테스트용
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	m_pLoading = CLoadingThread::Create(m_pGraphicDev, m_eChangeID);

	if (nullptr == m_pLoading)
		return E_FAIL;

	return S_OK;
}

_int CLoading::Update_Scene(const _float& fTimeDelta)
{
	m_pLoadingFG->Update_Pos(m_pLoading->Get_Clamp_Percent());

	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (true == m_pLoading->Get_Finish())
	{
		if (m_fLoadingDelay >= 2.f) {
			Engine::CScene* pScene = nullptr;
			switch (m_pLoading->Get_Loading())
			{
			case LOADING_STAGE:
				pScene = CStage::Create(m_pGraphicDev);

				if (nullptr == pScene)
					return -1;
				break;

			case LOADING_TEST:
				pScene = CTest::Create(m_pGraphicDev);

				if (nullptr == pScene)
					return -1;
				break;

			case LOADING_BOSS:
				break;
			}

			if (FAILED(CManagement::GetInstance()->Set_Scene(pScene)))
			{
				MSG_BOX("Stage Scene Failed");
				return -1;
			}
		}
		m_fLoadingDelay += fTimeDelta;
	}

	return iExit;
}

void CLoading::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);

	if (true == m_pLoading->Get_Finish()) {
		IMessageChannel::EVENT event; 
		event.strType = L"Loading_Success";

		m_pMessageChannel->Publish(event);
	}
}

void CLoading::Render_Scene()
{
	// debug 용


	_vec2		vPos{ 150.f, WINCY - 100.f };

	if (true == m_pLoading->Get_Finish()) {
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"로딩 완료!", &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
	}
	else {
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"로딩중.....", &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
	}

	//CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", m_pLoading->Get_String(), &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f));

}


HRESULT CLoading::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CLoading::Ready_UI_Layer(const _tchar* pLayerTag)
{

	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CLoadingBack::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingBack", pGameObject)))
		return E_FAIL;

	pGameObject = CLoadingCenter::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingCenter", pGameObject)))
		return E_FAIL;

	pGameObject = m_pLoadingFG = CLoadingFG::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingFG", pGameObject)))
		return E_FAIL;

	pGameObject =  CLoadingLogo::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingFG", pGameObject)))
		return E_FAIL;

	pGameObject = CLoadingCircle::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingFG", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CLoading::Ready_Prototype()
{
	return S_OK;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID)
{
	CLoading* pLoading = new CLoading(pGraphicDev, ChangeID);

	if (FAILED(pLoading->Ready_Scene()))
	{
		Safe_Release(pLoading);
		MSG_BOX("pLoading Create Failed");
		return nullptr;
	}

	return pLoading;
}

void CLoading::Free()
{
	Safe_Release(m_pLoading);

	CScene::Free();
}
