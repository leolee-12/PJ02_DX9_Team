#include "pch.h"
#include "CTutorial.h"
#include "CLightMgr.h"
#include "CMySkyBox.h"
#include "CPersistentMgr.h"
#include "CDInputMgr.h"
#include "CSoundMgr.h"
#include "CMainCamera.h"
#include "CMonsterN1.h"
#include "CMonsterN2.h"
#include "CMonsterN3.h"
#include "CMapLoader.h"
#include "CTileMgr.h"
#include "CMapObject.h"
#include "CGrass.h"
#include "CCollisionMgr.h"
#include "CBishop_Leshy.h"
#include "CMonsterB1.h"
#include "CBishop_Heket.h"
#include "CBishop_Kallamar.h"
#include "CBishop_Shamura.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CDynamicCamera.h"
#include "CTriggerPoint.h"
#include "CCutSceneMgr.h"
#include "CMapWarp.h"
#include "CWarp.h"
#include "CMapBorder.h"
#include "CLoading.h"
#include "CManagement.h"
#include "CFade.h"
#include "CBrute.h"

CTutorial::CTutorial(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CTutorial::~CTutorial()
{
}

HRESULT CTutorial::Ready_Scene()
{
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	Ready_Event();

	CCutSceneMgr::GetInstance()->Ready_CutsceneMgr(m_pMessageChannel);

	CUTSCENE tTutoCutScene;
	tTutoCutScene.strName = L"Tutorial_01";
	tTutoCutScene.vecSteps =
	{
		{_vec3(-10.f * 0.8f, 5.f, 135.f * 0.8f), 1.f, 1.f, L"Bishop_Heket", L"우리들 앞에 저들의 마지막 종자가 있다.\n다른 놈들은 이미 다 사냥하여 효수시켰지."},
		{_vec3(0.f * 0.8f, 5.f, 135.f * 0.8f), 1.f, 1.f, L"Bishop_Kallamar", L"이 마지막 번제로,\n이제 예언은 결코 달성할 수 없을 것이다."},
		{_vec3(-22.f * 0.8f, 5.f, 135.f * 0.8f), 1.f, 1.f, L"Bishop_Leshy", L"아래에 묶여있는 저 이단자는 풀려날 수 없다."},
		{_vec3(10.687412f * 0.8f, 5.f, 135.f * 0.8f), 1.f, 1.f, L"Bishop_Shamura", L"그리고 옛 신앙은 보존되리라."},
		{_vec3(-4.f, 2.f, 88.f), 1.5f, 0.5f, L"", L"", ADV_TIMED, 1.f},
		{_vec3(-4.f, 2.f, 88.f), 1.5f, 0.5f, L"Player", L"Start_Crying", ADV_IMMEDIATE},
		{_vec3(- 4.f, 2.f, 88.f), 1.5f, 0.5f, L"Brute", L"Brute_Move", ADV_EVENT, 0.f, L"Brute.RunEnd" },
		{_vec3(-4.f, 2.f, 88.f), 1.5f, 0.5f, L"Brute", L"Brute_Execute", ADV_EVENT, 0.f, L"Brute.ExecuteEnd" }

	};

	CCutSceneMgr::GetInstance()->Register_CutScene(tTutoCutScene);

	CUTSCENE tTutoIntro;
	tTutoIntro.strName = L"Tutorial_00";
	tTutoIntro.vecSteps =
	{
		{_vec3(-3.8f, 0.f, 3.6f), 1.f, 0.25f, L"FadeIn", L"",ADV_TIMED, 3.f}
	};

	CCutSceneMgr::GetInstance()->Register_CutScene(tTutoIntro);

	Ready_Light();

	CSoundMgr::GetInstance()->PlayBGM(L"00.Tutorial.mp3", 0.1f);

	return S_OK;
}

_int CTutorial::Update_Scene(const _float& fTimeDelta)
{
	Engine::CTransform* pPlayerTransform = CPersistentMgr::GetInstance()->Get_PlayerTransform();
	if (pPlayerTransform)
	{
		_vec3 vPlayerPos;
		pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
		Engine::CLightMgr::GetInstance()->Update_PointLights(vPlayerPos);
	}

	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (m_bSceneChangeFlag)
	{
		Engine::CScene* pLoading = CLoading::Create(m_pGraphicDev, LOADING_THEGATEWAY);

		if (nullptr == pLoading)
			return NOEVENT;

		if (FAILED(CManagement::GetInstance()->Set_Scene(pLoading)))
		{
			MSG_BOX("Stage Scene Failed");
			return NOEVENT;
		}
	}

	return iExit;
}

void CTutorial::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CTutorial::Render_Scene()
{
}

HRESULT CTutorial::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CMainCamera::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	static_cast<CMainCamera*>(pGameObject)->Set_Intro();

	if (FAILED(pLayer->Add_GameObject(L"MainCamera", pGameObject)))
		return E_FAIL;



	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CTutorial::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	// Map Load
	Engine::MAPDATA mapData;
	if (SUCCEEDED(Engine::CMapLoader::GetInstance()->LoadMapA(
		"../Bin/Resource/Maps/MapData/Tutorial.txt", mapData)))
	{
		// 맵 데이터의 skyType으로 SkyBox 생성
		pGameObject = CMySkyBox::Create(m_pGraphicDev, mapData.skyType);
		if (pGameObject)
			pLayer->Add_GameObject(L"SkyBox", pGameObject);

		// Tile
		CTileMgr::GetInstance()->Initialize(m_pGraphicDev, mapData);

		IMessageChannel::EVENT PlayerSpawnEvent;

		// Spawns (Monster)
		for (const auto& spawn : mapData.spawns)
		{
			switch (spawn.type)
			{
			case 0:
				CPersistentMgr::GetInstance()->Get_Player()->Set_Tied();
				CPersistentMgr::GetInstance()->Get_Player()->Set_Pos(_vec3(spawn.x, 0.f, spawn.z));
				pGameObject = CPersistentMgr::GetInstance()->Get_Player();

				if (nullptr == pGameObject)
					return E_FAIL;

				CPersistentMgr::GetInstance()->Get_Player()->Set_MessageChannel(m_pMessageChannel);

				if (FAILED(pLayer->Add_GameObject(L"Player", pGameObject)))
					return E_FAIL;

				pGameObject->AddRef();
				break;
			case 1:
				switch (spawn.monsterType)
				{
				case 0:
					// Bat | 일반몬스터 |
					break;
				case 1:
					// Worm | 일반몬스터 |
					pGameObject = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel);
					if (pGameObject)
					{
						Engine::CTransform* pTransform = dynamic_cast<Engine::CTransform*>(
							pGameObject->Get_Component(ID_DYNAMIC, L"Com_Transform"));
						if (pTransform)
							pTransform->Set_Pos(spawn.x, 0.f, spawn.z);
						pLayer->Add_GameObject(L"Monster", pGameObject);
					}
					break;
				case 2:
					// Humanoid | 일반몬스터 |
					break;
				case 3:
					// Amdusias | 중간보스 |
					break;
				case 4:
					// Rash | 최종보스 |
					break;
				case 5:
					// WaitingOne | 연출용 |
					break;
				case 6:
					pGameObject = CBishop_Leshy::Create(m_pGraphicDev, m_pMessageChannel, spawn);

					NULL_CHECK_RETURN(pGameObject, E_FAIL)

						if (FAILED(pLayer->Add_GameObject(L"Bishop_Leshy", pGameObject)))
							return E_FAIL;

					break;
				case 7:
					pGameObject = CBishop_Heket::Create(m_pGraphicDev, m_pMessageChannel, spawn);

					NULL_CHECK_RETURN(pGameObject, E_FAIL)

						if (FAILED(pLayer->Add_GameObject(L"Bishop_Heket", pGameObject)))
							return E_FAIL;

					break;

				case 8:
					pGameObject = CBishop_Kallamar::Create(m_pGraphicDev, m_pMessageChannel, spawn);

					NULL_CHECK_RETURN(pGameObject, E_FAIL)

						if (FAILED(pLayer->Add_GameObject(L"Bishop_Kallamar", pGameObject)))
							return E_FAIL;

					break;

				case 9:
					pGameObject = CBishop_Shamura::Create(m_pGraphicDev, m_pMessageChannel, spawn);

					NULL_CHECK_RETURN(pGameObject, E_FAIL)

						if (FAILED(pLayer->Add_GameObject(L"Bishop_Shamura", pGameObject)))
							return E_FAIL;

					break;
				}
				break;
			default:
				MSG_BOX("스폰섹션 타입오류");
				return E_FAIL;
			}
		}

		// Objects
		for (const auto& obj : mapData.objects)
		{
			if (obj.category == "Grass")
			{
				pGameObject = CGrass::Create(m_pGraphicDev, obj, m_pMessageChannel);
				if (pGameObject)
					pLayer->Add_GameObject(L"Grass", pGameObject);
			}
			else
			{
				pGameObject = CMapObject::Create(m_pGraphicDev, obj);
				if (pGameObject)
					pLayer->Add_GameObject(L"MapObject", pGameObject);
			}
		}
		// Process Lights - Point Light 생성
		for (const auto& light : mapData.lights)
		{
			Engine::CLightMgr::GetInstance()->Ready_PointLight(m_pGraphicDev, light);
		}
		// Process MapWarps - Group by PairId and create CMapWarp
		std::map<_int, std::vector<MAPWARPDATA>> warpGroups;
		for (const auto& warp : mapData.mapWarps)
		{
			warpGroups[warp.pairId].push_back(warp);
		}
		for (const auto& group : warpGroups)
		{
			if (group.second.size() >= 2)
			{
				const MAPWARPDATA& w1 = group.second[0];
				const MAPWARPDATA& w2 = group.second[1];

				_vec3 pos1 = { w1.x, 0.f, w1.z };
				_vec3 pos2 = { w2.x, 0.f, w2.z };

				pGameObject = CMapWarp::Create(m_pGraphicDev, m_pMessageChannel,
					pos1, w1.direction, pos2, w2.direction);

				if (pGameObject)
					pLayer->Add_GameObject(L"MapWarp", pGameObject);
			}
		}
		// Border
		for (const auto& col : mapData.collisions)
		{
			pGameObject = CMapBorder::Create(m_pGraphicDev, col.x, col.z);
			if (pGameObject)
				pLayer->Add_GameObject(L"MapBorder", pGameObject);
		}
	}
	else
	{
		// 맵 로드 실패 시 기본 SkyBox (Day) 생성
		pGameObject = CMySkyBox::Create(m_pGraphicDev, 0);
		if (pGameObject)
			pLayer->Add_GameObject(L"SkyBox", pGameObject);
	}

	// 디버그용


	_vec3 vTriggerPos, vTriggerHalfSize;
	vTriggerPos = { -4.f, 2.f, 88.f };
	vTriggerHalfSize = { 5.f, 5.f, 5.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"Tutorial_01");

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	vTriggerPos = { -4.f, 0.f, 4.f };
	vTriggerHalfSize = { 5.f, 5.f, 5.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"Tutorial_00", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	CMonsterN1* pTemp = nullptr;
	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -8.f, 0.f, 92.5f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ 1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ 0.f, 0.f, 92.5f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ -1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -8.5f, 0.f, 82.f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ 1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ 0.5f, 0.f, 82.f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ -1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -13.f, 0.f, 87.5f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ 1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	pGameObject = CBrute::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ 8.f, 0.f, 90.f });
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CTutorial::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CFade::Create(m_pGraphicDev, m_pMessageChannel);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"Fade", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

void CTutorial::Ready_Event()
{
	m_hmapSubHandles.insert({ L"CutScene.End", m_pMessageChannel->Subscribe(L"CutScene.End", [this](const IMessageChannel::EVENT& Event) {
	if (any_cast<wstring>(Event.hmapData.find(L"SceneName")->second) == L"Tutorial_01")
	{
		m_bSceneChangeFlag = true;
	}
	}) });
}


HRESULT CTutorial::Ready_Light()
{
	D3DLIGHT9	tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(0.9f, 0.1f, 0.1f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.4f, 0.2f, 0.6f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.3f, 0.12f, 0.4f, 1.f);

	tLightInfo.Direction = { 1.f, -1.f, 1.f };

	if (FAILED(CLightMgr::GetInstance()->Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;

	return S_OK;
}

CTutorial* CTutorial::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTutorial* pTest = new CTutorial(pGraphicDev);

	if (FAILED(pTest->Ready_Scene()))
	{
		Safe_Release(pTest);
		MSG_BOX("pCTutorial Create Failed");
		return nullptr;
	}

	return pTest;
}

void CTutorial::Free()
{
	CSoundMgr::GetInstance()->StopAll();
	CCollisionMgr::GetInstance()->Reset_For_SceneChange();
	CTileMgr::GetInstance()->Reset_For_SceneChange();
	CLightMgr::GetInstance()->DestroyInstance();
	CScene::Free();
}
