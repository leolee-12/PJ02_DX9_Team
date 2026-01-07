#include "pch.h"
#include "CStage.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CDynamicCamera.h"
// #include "CSkyBox.h"  // CMySkyBox로 대체
#include "CTestEffect.h"
#include "CLightMgr.h"
#include "CCollisionMgr.h"
#include "CLoading.h"
#include "CManagement.h"
#include "CPersistentMgr.h"
#include <CMonsterN1.h>
#include "CMapLoader.h"
#include "CTile.h"
#include "CTileMgr.h"
#include "CMapObject.h"
#include "CGrass.h"
#include "CMySkyBox.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene()
{
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	// �׽�Ʈ��

	/*m_pLoading = CLoadingThread::Create(m_pGraphicDev, CLoadingThread::LOADING_STAGE);

	if (nullptr == m_pLoading)
		return E_FAIL;*/

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);


	/*if (true == m_pLoading->Get_Finish())
	{*/
		if (GetAsyncKeyState('M'))
		{
			/*auto iter = m_mapLayer.find(L"Const_Layer");
			if (iter == m_mapLayer.end()) { return -1; }

			Engine::CScene* pTest = CTest::Create(m_pGraphicDev, iter->second);
			iter->second->AddRef();
			if (nullptr == pTest)
				return -1;*/

			/*if (FAILED(CManagement::GetInstance()->Set_Scene(pTest)))
			{
				MSG_BOX("Stage Scene Failed");
				return -1;
			}*/
			Engine::CScene* pLoading = CLoading::Create(m_pGraphicDev, LOADING_TEST);
			
			if (nullptr == pLoading)
				return -1;

			if (FAILED(CManagement::GetInstance()->Set_Scene(pLoading)))
			{
				MSG_BOX("Stage Scene Failed");
				return -1;
			}
		}
	//}

	return iExit;
}

void CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);

	IMessageChannel::EVENT EventTest;
	EventTest.strType = L"Start_Game";
	EventTest.eOBJID = Engine::OID_PLAYER;

	m_pMessageChannel->Publish(EventTest);

	EventTest.strType = L"Monster.Move";
	EventTest.eOBJID = Engine::OID_MONSTER;

	m_pMessageChannel->Publish(EventTest);
}

void CStage::Render_Scene()
{
}

HRESULT CStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	/*pGameObject = CSkyBox::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"SkyBox", pGameObject)))
		return E_FAIL;*/

	_vec3   vEye{ 0.f, 10.f, -10.f };
	_vec3   vAt{ 0.f, 0.f, 1.f };
	_vec3   vUp{ 0.f, 1.f, 0.f };

	// DynamicCamera
	pGameObject = CDynamicCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"DynamicCamera", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	// Load Map Data
	Engine::MAPDATA mapData;
	if (SUCCEEDED(Engine::CMapLoader::GetInstance()->LoadMapA("../../Maps/MapData/Tutorial_test.txt", mapData)))
	{
		// 맵 데이터의 skyType으로 SkyBox 생성
		pGameObject = CMySkyBox::Create(m_pGraphicDev, mapData.skyType);
		if (pGameObject)
		{
			pLayer->Add_GameObject(L"SkyBox", pGameObject);
		}

		// Initialize Tile Manager
		if (FAILED(CTileMgr::GetInstance()->Initialize(m_pGraphicDev, mapData)))
		{
			MSG_BOX("Failed to initialize TileMgr");
		}

		// Process Spawns
		for (const auto& spawn : mapData.spawns)
		{
			if (spawn.type == 0) // Player spawn
			{
				// Set player position later
			}
			else if (spawn.type == 1) // Monster spawn
			{
				pGameObject = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel);
				if (pGameObject)
				{
					Engine::CTransform* pTransform = dynamic_cast<Engine::CTransform*>(
						pGameObject->Get_Component(ID_DYNAMIC, L"Com_Transform"));
					if (pTransform)
					{
						pTransform->Set_Pos(spawn.x, 0.f, spawn.z);
					}
					pLayer->Add_GameObject(L"Monster", pGameObject);
				}
			}
		}

		// Process Objects
		for (const auto& obj : mapData.objects)
		{
			if (obj.category == "Grass")
			{
				pGameObject = CGrass::Create(m_pGraphicDev, obj);
				if (pGameObject)
				{
					pLayer->Add_GameObject(L"Grass", pGameObject);
				}
			}
			else
			{
				pGameObject = CMapObject::Create(m_pGraphicDev, obj);
				if (pGameObject)
				{
					pLayer->Add_GameObject(L"MapObject", pGameObject);
				}
			}
		}

	}
	else
	{
		// 맵 로드 실패 시 기존 Terrain 사용
		// 기본 SkyBox (Day) 생성
		pGameObject = CMySkyBox::Create(m_pGraphicDev, 0);
		if (pGameObject)
		{
			pLayer->Add_GameObject(L"SkyBox", pGameObject);
		}

		pGameObject = CTerrain::Create(m_pGraphicDev);
		if (nullptr == pGameObject)
			return E_FAIL;
		if (FAILED(pLayer->Add_GameObject(L"Terrain", pGameObject)))
			return E_FAIL;

		pGameObject = CTerrainWall::Create(m_pGraphicDev);
		if (nullptr == pGameObject)
			return E_FAIL;
		if (FAILED(pLayer->Add_GameObject(L"TerrainWall", pGameObject)))
			return E_FAIL;

		// Original monster spawning
		pGameObject = CMonster::Create(m_pGraphicDev, m_pMessageChannel);
		if (nullptr == pGameObject)
			return E_FAIL;
		if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
			return E_FAIL;

		for (_uint i = 0; i < 20; ++i)
		{
			pGameObject = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel);
			if (nullptr == pGameObject)
				return E_FAIL;
			if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
				return E_FAIL;
		}
	}

	pGameObject = CPersistentMgr::GetInstance()->Get_GlobalObjects(GOBJ_PLAYER);

	if (nullptr == pGameObject)
		return E_FAIL;

	pGameObject->Set_MessageChannel(m_pMessageChannel);

	if (FAILED(pLayer->Add_GameObject(L"Player", pGameObject)))
		return E_FAIL;

	pGameObject->AddRef();

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CStage::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	//CGameObject* pGameObject = nullptr;
	//
	//for (_int i = 0; i < 50; ++i)
	//{
	//	pGameObject = CTestEffect::Create(m_pGraphicDev);
	//
	//	if (nullptr == pGameObject)
	//		return E_FAIL;
	//
	//	if (FAILED(pLayer->Add_GameObject(L"Effect", pGameObject)))
	//		return E_FAIL;
	//}

	m_mapLayer.insert({ pLayerTag , pLayer });


	return S_OK;
}

HRESULT CStage::Ready_Const_Layer()
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;


	m_mapLayer.insert({ L"Const_Layer" , pLayer});
	return S_OK;
}

HRESULT CStage::Ready_Light()
{
	D3DLIGHT9	tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tLightInfo.Direction = { 1.f, -1.f, 1.f };

	if (FAILED(CLightMgr::GetInstance()->Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;


	return S_OK;
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage* pLogo = new CStage(pGraphicDev);

	if (FAILED(pLogo->Ready_Scene()))
	{
		Safe_Release(pLogo);
		MSG_BOX("pLogo Create Failed");
		return nullptr;
	}

	return pLogo;
}

void CStage::Free()
{
	//Safe_Release(m_pLoading);
	CCollisionMgr::GetInstance()->Reset_For_SceneChange();
	CScene::Free();
}
