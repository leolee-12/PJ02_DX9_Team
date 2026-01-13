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
#include "CKnuckleBone.h"
#include "CDungeon.h"
#include "CMultiLoadingThread.h"
#include "Engine_Define.h"
#include "CPersistentMgr.h"
#include "CN1_AI.h"
#include "CN2_AI.h"
#include "CN3_AI.h"
#include "CTutorial.h"
#include "CTheGateWay.h"
#include "CVillage.h"
#include "CLogo.h"
#include "CRealDungeon.h"
#include "CAmdusiasRoom.h"
#include "CLeshyRoom.h"
//#include "LoadObjectList.h"



CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID)
	: CScene(pGraphicDev), m_pLoading(nullptr), m_eChangeID(ChangeID), m_fLoadingDelay(0.f)
	, m_iCompletedCount(0), m_pLoadingFG(nullptr)
	, m_fLoadingPersent(0.f), m_iLoadCount(0), m_iTotalCount(0)
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

	//if (FAILED(Ready_Prototype()))
	//	return E_FAIL;

	//m_pLoading = CLoadingThread::Create(m_pGraphicDev, m_eChangeID);

	m_pLoading = CMultiLoadingThread::Create(m_pGraphicDev, m_eChangeID);

	if (nullptr == m_pLoading)
		return E_FAIL;

	m_iTotalCount = m_pLoading->Get_TotalCount();

	return S_OK;
}

_int CLoading::Update_Scene(const _float& fTimeDelta)
{
	//Excute_Ready_Texture();
	//m_iCompletedCount;
	//m_iTotalCount;
	Update_Count();

	//텍스쳐 로딩이끝나면 -> 게이지 50퍼보이게 프로토로딩이끝나면 -> 게이지 100퍼 

	//_float LoadingClamp = _float(m_iLoadCount) / _float(m_iTotalCount);
	_float ProtoClamp = _float(m_iCompletedCount) / _float(m_iTotalCount);

	m_fLoadingPersent = ProtoClamp;

	if (m_fLoadingPersent < 0)
	{
		m_fLoadingPersent = 0.f;
	}
	else if (m_fLoadingPersent > 1.f)
	{
		m_fLoadingPersent = 1.f;
	}


	m_pLoadingFG->Update_Pos(m_fLoadingPersent);

	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	//if (true == m_pLoading->Get_Finish())
	if (m_iTotalCount == m_iCompletedCount)
	{
		if (m_fLoadingDelay >= 1.f) {
			Engine::CScene* pScene = nullptr;
			switch (m_pLoading->Get_Loading())
			{
					case LOADING_LOGO:
						pScene = CLogo::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
					case LOADING_STAGE:
						pScene = CStage::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
					case LOADING_BOSS:
						// 이건 없어용~~
						break;
					case LOADING_TEST:
						pScene = CTest::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
					case LOADING_KNUCKLEBONE:
						pScene = CKnuckleBone::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
					case LOADING_DUNGEON:
						pScene = CDungeon::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
					case LOADING_TUTORIAL:
						pScene = CTutorial::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
					case LOADING_THEGATEWAY:
						pScene = CTheGateway::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
					case LOADING_VILLAGE:
						pScene = CVillage::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
					case LOADING_REALDUNGEON:
						pScene = CRealDungeon::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
					case LOADING_AMDUSIASROOM:
						pScene = CAmdusiasRoom::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
					case LOADING_LESHYROOM:
						pScene = CLeshyRoom::Create(m_pGraphicDev);

						if (nullptr == pScene)
							return -1;
						break;
			};

			if (FAILED(CManagement::GetInstance()->Set_Scene(pScene)))
			{
				MSG_BOX("Stage Scene Failed");
				return -1;
			}
		}
		_float fClampedDelta = min(fTimeDelta, 0.1f);  // 최대 100ms로 제한
		m_fLoadingDelay += fClampedDelta;
	}

	return iExit;
}

void CLoading::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);

	//if (true == m_pLoading->Get_Finish()) {
	//	IMessageChannel::EVENT event; 
	//	event.strType = L"Loading_Success";
	//
	//	m_pMessageChannel->Publish(event);
	//}
}

void CLoading::Render_Scene()
{
	// debug 용

	_int iRenderPersent = _int(m_fLoadingPersent * 100.f);

	_tchar szPersent[64] = L"";

	swprintf_s(szPersent, L"\n%d %%", iRenderPersent);

	_vec2		vPos{ 150.f, WINCY - 100.f };
	_vec2		vPos2{ 150.f + 50.f, WINCY - 100.f };

	if (m_iTotalCount / 2 < m_iCompletedCount) {
		if (m_iTotalCount == m_iCompletedCount) {
			CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"로딩 완료!", &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
			CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", szPersent, &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
		}
		else {
			CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"오브젝트 생성중", &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
			CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", szPersent, &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
		}
	}
	else {
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"텍스쳐 생성중", &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", szPersent, &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
	}

	
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

	if (FAILED(pLayer->Add_GameObject(L"CLoadingLogo", pGameObject)))
		return E_FAIL;

	pGameObject = CLoadingCircle::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingCircle", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

void CLoading::Update_Count()
{
	//m_iLoadCount = m_pLoading->Get_LoadCount();
	m_iCompletedCount = m_pLoading->Get_CompletedCount();
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
