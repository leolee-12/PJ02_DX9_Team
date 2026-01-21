#include "pch.h"
#include "CDungeon.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CDynamicCamera.h"
#include "CMySkyBox.h"
#include "CPersistentMgr.h"
#include "CDungeonBack.h"
#include "CDInputMgr.h"
#include "CDungeonIcon.h"
#include "CLightMgr.h"
#include "CDungeonLine.h"
#include "CPlayerHP.h"
#include "CSoundMgr.h"
#include "CMainCamera.h"
#include "CMonsterN1.h"
#include "CMonsterN2.h"
#include "CMonsterN3.h"
#include "CMonsterB1.h"
#include "CMonsterB2.h"
#include "CMapLoader.h"
#include "CTileMgr.h"
#include "CMapObject.h"
#include "CGrass.h"
#include "CCollisionMgr.h"
#include "CGauge.h"
#include "CBishop_Leshy.h"
#include "CBishop_Heket.h"
#include "CBishop_Kallamar.h"
#include "CBishop_Shamura.h"
#include "CCookingUIController.h"
#include "CCookingMiniGameUI.h"
#include "CProjectile.h"
#include "CMapWarp.h"
#include "CWarp.h"
#include "CMapBorder.h"
#include "CRatau.h"
#include "CTarotSeller.h"
#include "CBrute.h"
#include "CBossHpBar.h"
#include "CTriggerPoint.h"
#include "CCutSceneMgr.h"
#include "CFollower.h"
#include "CFade.h"
#include "CLoading.h"
#include "CManagement.h"
#include "CChest.h"
#include "CEffectMgr.h"

CDungeon::CDungeon(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CDungeon::~CDungeon()
{
}

HRESULT CDungeon::Ready_Scene()
{
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	Ready_Light();

	Ready_Event();

	CCutSceneMgr::GetInstance()->Ready_CutsceneMgr(m_pMessageChannel);

	CEffectMgr::GetInstance()->Ready_EffectMgr(m_pGraphicDev);

	CUTSCENE tDungeonScene;
	tDungeonScene.strName = L"Ratau_00";
	tDungeonScene.vecSteps =
	{
		{_vec3(123.5f, 0.f, 12.f), 0.75f, 0.5f, L"Ratau", L"Ratau_Intro", ADV_EVENT, 0.f, L"Ratau.Done"},
		{_vec3(123.5f, 0.f, 12.f), 1.5f, 0.5f, L"Ratau", L"두려워 마시오! 나는 라타우. 한때 그대처럼\n선택받은 그릇이었소."},
		{_vec3(123.5f, 0.f, 12.f), 1.5f, 0.5f, L"Ratau", L"우리는 현재 옛 신앙의 땅 깊은곳에 있고,\n엄청난 위험에 빠져있소."},
		{_vec3(123.5f, 0.f, 12.f), 1.5f, 0.5f, L"Ratau", L"내 가르침으로 그대를 인도하겠소. 숲을 향해\n계속 나아가시오. 내 가까이서 있을테니."},
		{_vec3(123.5f, 0.f, 12.f), 0.75f, 0.5f, L"Ratau", L"Ratau_Outro", ADV_EVENT, 0.f, L"Ratau.Done"}
	};

	CCutSceneMgr::GetInstance()->Register_CutScene(tDungeonScene);

	tDungeonScene.strName = L"Ratau_01";
	tDungeonScene.vecSteps =
	{
		{_vec3(-236.f, 0.f, 4.f), 1.f, 1.f, L"Scene", L"Create_Ratau", ADV_IMMEDIATE},
		{_vec3(-236.f, 0.f, 4.f), 1.f, 1.f, L"Player", L"LookforCam", ADV_TIMED, 2.f},
		{_vec3(-231.f, 0.f, 4.f), 0.75f, 0.5f, L"Ratau", L"Ratau_Intro", ADV_EVENT, 0.f, L"Ratau.Done"},
		{_vec3(-231.f, 0.f, 4.f), 1.5f, 0.5f, L"Ratau", L"보시오! 제물로 바쳐지는 또다른 가녀린 영혼이오."},
		{_vec3(-231.f, 0.f, 4.f), 1.5f, 0.5f, L"Ratau", L"저 자를 구출하면 분명 우리 교단에 들어와줄 것이오."},
		{_vec3(-231.f, 0.f, 4.f), 0.75f, 0.5f, L"Ratau", L"Ratau_Outro", ADV_EVENT, 0.f, L"Ratau.Done"}
	};

	CCutSceneMgr::GetInstance()->Register_CutScene(tDungeonScene);

	CSoundMgr::GetInstance()->PlayBGM(L"03.Dungeon.mp3", 0.1f);

	return S_OK;
}

_int CDungeon::Update_Scene(const _float& fTimeDelta)
{
	Engine::CTransform* pPlayerTransform = CPersistentMgr::GetInstance()->Get_PlayerTransform();
	if (pPlayerTransform)
	{
		_vec3 vPlayerPos;
		pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
		Engine::CLightMgr::GetInstance()->Update_PointLights(vPlayerPos);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_R))
	{
		IMessageChannel::EVENT event;
		event.strType = L"Select";

		m_pMessageChannel->Publish(event);

		CSoundMgr::GetInstance()->Play(L"AAAK.wav", SOUND_EFFECT, 0.1f);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_U))
	{
		IMessageChannel::EVENT event;
		event.strType = L"Choose";
		event.hmapData.insert({ L"Look_Stage", CDungeonLine::DL_1 });

		m_pMessageChannel->Publish(event);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_I))
	{
		IMessageChannel::EVENT event;
		event.strType = L"Enter";
		event.hmapData.insert({ L"Look_Stage", CDungeonLine::DL_1 });

		m_pMessageChannel->Publish(event);
	}

	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (m_bSceneChangeFlag)
	{
		Engine::CScene* pLoading = CLoading::Create(m_pGraphicDev, LOADING_VILLAGE);

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

void CDungeon::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);

	CCollisionMgr::GetInstance()->Check_Collisions(fTimeDelta);
}

void CDungeon::Render_Scene()
{
}

HRESULT CDungeon::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CMainCamera::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"MainCamera", pGameObject)))
		return E_FAIL;


	pGameObject = CChest::Create(m_pGraphicDev, m_pMessageChannel, _vec3(-148.f, 0.f, 14.1f), 10);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"Chest", pGameObject)))
		return E_FAIL;



	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CDungeon::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	// Map Load
	Engine::MAPDATA mapData;
	if (SUCCEEDED(Engine::CMapLoader::GetInstance()->LoadMapA(
		"../Bin/Resource/Maps/MapData/Dungeon.txt", mapData)))
	{

		pGameObject = CMySkyBox::Create(m_pGraphicDev, mapData.skyType);
		if (pGameObject)
			pLayer->Add_GameObject(L"SkyBox", pGameObject);

		// Tile
		CTileMgr::GetInstance()->Initialize(m_pGraphicDev, mapData);

		// Spawns (Monster)
		for (const auto& spawn : mapData.spawns)
		{
			switch (spawn.type)
			{
			case 0:
				CPersistentMgr::GetInstance()->Get_Player()->Set_Pos(_vec3(spawn.x * 0.8f, 0.f, spawn.z * 0.8f));
				CPersistentMgr::GetInstance()->Get_Player()->Set_Reborn();
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
					pGameObject = CMonsterN3::Create(m_pGraphicDev, m_pMessageChannel);
					if (pGameObject)
					{
						Engine::CTransform* pTransform = dynamic_cast<Engine::CTransform*>(
							pGameObject->Get_Component(ID_DYNAMIC, L"Com_Transform"));
						if (pTransform)
						{
							pTransform->Set_Pos(spawn.x * 0.8f, 0.f, spawn.z * 0.8f);
							pTransform->Update_Component(0.f);
						}
						pLayer->Add_GameObject(L"Monster", pGameObject);
					}
					break;
					break;
				case 1:
					// Worm | 일반몬스터 |
					pGameObject = CMonsterN2::Create(m_pGraphicDev, m_pMessageChannel);
					if (pGameObject)
					{
						Engine::CTransform* pTransform = dynamic_cast<Engine::CTransform*>(
							pGameObject->Get_Component(ID_DYNAMIC, L"Com_Transform"));
						if (pTransform)
						{
							pTransform->Set_Pos(spawn.x * 0.8f, 0.f, spawn.z * 0.8f);
							pTransform->Update_Component(0.f);
						}
						pLayer->Add_GameObject(L"Monster", pGameObject);
					}
					break;
				case 2:
					// Humanoid | 일반몬스터 |
					pGameObject = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel);
					if (pGameObject)
					{
						Engine::CTransform* pTransform = dynamic_cast<Engine::CTransform*>(
							pGameObject->Get_Component(ID_DYNAMIC, L"Com_Transform"));
						if (pTransform)
						{
							pTransform->Set_Pos(spawn.x * 0.8f, 0.f, spawn.z * 0.8f);
							pTransform->Update_Component(0.f);
						}
						pLayer->Add_GameObject(L"Monster", pGameObject);
					}
					break;
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

	for (_uint i = 0; i < 20; ++i)
	{
		//pGameObject = CMonsterN2::Create(m_pGraphicDev, m_pMessageChannel);
		//
		//NULL_CHECK_RETURN(pGameObject, E_FAIL)
		//
		//	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		//		return E_FAIL;
	}

	//pGameObject = CMonsterB1::Create(m_pGraphicDev, m_pMessageChannel);
	//
	//NULL_CHECK_RETURN(pGameObject, E_FAIL)
	//
	//	if (FAILED(pLayer->Add_GameObject(L"Boss", pGameObject)))
	//		return E_FAIL;

	//pGameObject = CMonsterB2::Create(m_pGraphicDev, m_pMessageChannel);
	//
	//NULL_CHECK_RETURN(pGameObject, E_FAIL)
	//
	//	if (FAILED(pLayer->Add_GameObject(L"Boss", pGameObject)))
	//		return E_FAIL;

	//----------------------------튜토리얼 몬스터 배치---------------------------
	CMonsterN1* pTemp = nullptr;
	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -8.f, 0.f, 92.5f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ 1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		return E_FAIL;

	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ 0.f, 0.f, 92.5f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ -1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		return E_FAIL;

	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -8.5f, 0.f, 82.f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ 1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		return E_FAIL;

	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ 0.5f, 0.f, 82.f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ -1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		return E_FAIL;

	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -13.f, 0.f, 87.5f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ 1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		return E_FAIL;
	//---------------------------------------------------------------------------

	//----------------------------라타우 컷씬2 방 배치---------------------------
	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -240.f, 0.f, 8.f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ 1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		return E_FAIL;

	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -236.f, 0.f, 16.f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ -1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		return E_FAIL;

	pGameObject = pTemp = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -232.f, 0.f, 8.f }, CMonsterN1::N1S_PRAY);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	pTemp->Set_Dir(_vec3{ -1.f, 0.f, 0.f });
	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		return E_FAIL;

	pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower3Texture", _vec3(-236.f, 0.f, 11.5f ), CFollower::FOLLOWER_UNCONVERT);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;
	//----------------------------라타우 컷씬2 방 몬스터 배치---------------------------

	pGameObject = CRatau::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ 123.5f, 0.f, 12.f });

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	/*pGameObject = CRatau::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -231.f, 0.f, 4.f });

	NULL_CHECK_RETURN(pGameObject, E_FAIL)

		if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
			return E_FAIL;*/

	pGameObject = CTarotSeller::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ 330.f, 0.f, 11.5f });

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	_vec3 vTriggerPos, vTriggerHalfSize;
	vTriggerPos = { 121.f, 0.f, 12.f };
	vTriggerHalfSize = { 3.f, 3.f, 3.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"Ratau_00", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	vTriggerPos = { -236.f, 0.f, 4.f };
	vTriggerHalfSize = { 3.f, 3.f, 3.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"Ratau_01", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	vTriggerPos = { -236.f, 0.f, 27.f };
	vTriggerHalfSize = { 3.f, 3.f, 3.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_SCENE, L"Village", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CDungeon::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	//////////////////////////////////////////////////////
	//플레이어 UI
	pGameObject = CPersistentMgr::GetInstance()->Get_PlayerHPUI();

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"PlayerHP", pGameObject)))
		return E_FAIL;
	pGameObject->AddRef();

	pGameObject = CPersistentMgr::GetInstance()->Get_Gauge();

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Gauge", pGameObject)))
		return E_FAIL;
	pGameObject->AddRef();
	//플레이어 UI
	////////////////////////////////////////////////////////

	pGameObject = CDungeonBack::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"SelectBack", pGameObject)))
		return E_FAIL;

	pGameObject = CDungeonIcon::Create(m_pGraphicDev, m_pMessageChannel, CDungeonIcon::DI_STAGE1);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"SelectIcon", pGameObject)))
		return E_FAIL;

	pGameObject = CDungeonIcon::Create(m_pGraphicDev, m_pMessageChannel, CDungeonIcon::DI_STAGE2);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"SelectIcon", pGameObject)))
		return E_FAIL;

	pGameObject = CDungeonIcon::Create(m_pGraphicDev, m_pMessageChannel, CDungeonIcon::DI_STAGE3);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"SelectIcon", pGameObject)))
		return E_FAIL;

	pGameObject = CDungeonLine::Create(m_pGraphicDev, m_pMessageChannel, CDungeonLine::DL_1);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"SelectLine", pGameObject)))
		return E_FAIL;

	pGameObject = CDungeonLine::Create(m_pGraphicDev, m_pMessageChannel, CDungeonLine::DL_2);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"SelectLine", pGameObject)))
		return E_FAIL;

	pGameObject = CCookingUIController::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"CookingUIController", pGameObject)))
		return E_FAIL;

	pGameObject = CFade::Create(m_pGraphicDev, m_pMessageChannel);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"Fade", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CDungeon::Ready_Const_Layer()
{
	return S_OK;
}

HRESULT CDungeon::Ready_Light()
{
	D3DLIGHT9	tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(0.4f, 0.3f, 0.85f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.4f, 0.2f, 0.6f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.3f, 0.12f, 0.4f, 1.f);

	tLightInfo.Direction = { 1.f, -1.f, 1.f };

	if (FAILED(CLightMgr::GetInstance()->Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;

	return S_OK;
}

void CDungeon::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Obj_Add", m_pMessageChannel->Subscribe(L"Obj.Add", [this](const IMessageChannel::EVENT& Event) {
	{
		CGameObject* pGObj = any_cast<CGameObject*>(Event.hmapData.find(L"Obj")->second);

		if (pGObj != nullptr)
		{
			wstring strLayerTag = any_cast<const _tchar*>(Event.hmapData.find(L"LayerTag")->second);
			wstring strObjTag = any_cast<wstring>(Event.hmapData.find(L"ObjTag")->second);
			auto iter = m_mapLayer.find(strLayerTag);

			if(iter != m_mapLayer.end())
				iter->second->Add_GameObject(strObjTag, pGObj);
		}
	}
	}) });

	m_hmapSubHandles.insert({ L"Dialogue", m_pMessageChannel->Subscribe(L"CutScene.Dialogue", [this](const IMessageChannel::EVENT& Event)
		{
			auto CinemaTargetNameiter = Event.hmapData.find(L"CinemaTargetName");
			if (CinemaTargetNameiter == Event.hmapData.end()) { return; }
			auto Dothisiter = Event.hmapData.find(L"Dothis");
			if (Dothisiter == Event.hmapData.end()) { return; }
			if (any_cast<wstring>(CinemaTargetNameiter->second) == L"Scene")
			{
				wstring strDothis = any_cast<wstring>(Dothisiter->second);
				if (strDothis == L"Create_Ratau") {

					CGameObject* pGameObject = nullptr;

					pGameObject = CRatau::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ -231.f, 0.f, 4.f });

					if (pGameObject == nullptr) { return; }

					auto iter = m_mapLayer.find(L"GameLogic_Layer");
					if (iter == m_mapLayer.end()) { return; }

					iter->second->Add_GameObject(L"NPC", pGameObject);
					return;
				}
			}
		}
	) });

	m_hmapSubHandles.insert({ L"Trigger.Activate", m_pMessageChannel->Subscribe(L"Trigger.Activate", [this](const IMessageChannel::EVENT& Event)
		{
			auto TIDiter = Event.hmapData.find(L"Trigger_TID");
			if (TIDiter == Event.hmapData.end()) { return; }
			auto TriggetNameiter = Event.hmapData.find(L"Trigger_Name");
			if (TriggetNameiter == Event.hmapData.end()) { return; }
			if (any_cast<Trigger::TRIGGERID>(TIDiter->second) == Trigger::TI_SCENE)
			{
				if (any_cast<wstring>(TriggetNameiter->second) == L"Village")
				{
					m_bSceneChangeFlag = true;
				}
			}
		}
	) });
}

CDungeon* CDungeon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDungeon* pTest = new CDungeon(pGraphicDev);

	if (FAILED(pTest->Ready_Scene()))
	{
		Safe_Release(pTest);
		MSG_BOX("pDungeon Create Failed");
		return nullptr;
	}

	return pTest;
}

void CDungeon::Free()
{
	CCollisionMgr::GetInstance()->Reset_For_SceneChange();
	CTileMgr::GetInstance()->Reset_For_SceneChange();
	CSoundMgr::GetInstance()->StopAll();
	CLightMgr::GetInstance()->DestroyInstance();
	CScene::Free();
}
