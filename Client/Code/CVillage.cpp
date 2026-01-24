#include "pch.h"
#include "CVillage.h"
#include "CCollisionMgr.h"
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
#include "CBishop_Leshy.h"
#include "CMonsterB1.h"
#include "CBishop_Heket.h"
#include "CBishop_Kallamar.h"
#include "CBishop_Shamura.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CDynamicCamera.h"
#include "CMapWarp.h"
#include "CWarp.h"
#include "CMapBorder.h"
#include "CFollower.h"
#include "CBreakableRock.h"
#include "CBreakableTree.h"
#include "CRatau.h"
#include "CCutSceneMgr.h"
#include "CTriggerPoint.h"
#include "CFade.h"
#include "CLoading.h"
#include "CManagement.h"
#include "CBuilding.h"
#include "CCookingUIController.h"
#include "CEffectMgr.h"
#include "CNPCCommandUI.h"
#include "CFoodReviewUI.h"

CVillage::CVillage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CVillage::~CVillage()
{
}

HRESULT CVillage::Ready_Scene()
{
	m_pMessageChannel = CStageMessage::Create();

	CInteractMgr::GetInstance()->Ready_InteractMgr();

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	Ready_Light();

	Ready_Event_Village();

	Ready_Event();


	CCutSceneMgr::GetInstance()->Ready_CutsceneMgr(m_pMessageChannel);

	CUTSCENE tDungeonScene;
	tDungeonScene.strName = L"Ratau_02";
	tDungeonScene.vecSteps =
	{
		{_vec3(207.6f, 0.f, 84.f), 0.75f, 0.5f, L"Player", L"LookforCam", ADV_IMMEDIATE},
		{_vec3(207.6f, 0.f, 84.f), 0.75f, 0.5f, L"Ratau", L"Ratau_Intro", ADV_EVENT, 0.f, L"Ratau.Done"},
		{_vec3(207.6f, 0.f, 84.f), 1.5f, 0.5f, L"Ratau", L"이 텅 빈 땅은 이제 그대의 것이오. 이 부서진 유적은\n새로운 교단을 세우기 위한 지점이 될 것이오."},
		{_vec3(207.6f, 0.f, 84.f), 1.5f, 0.5f, L"Ratau", L"해야할 것이 많소. 우선 가련한 영혼들을 교육시켜\n교단에 들여오게 합시다."},
		{_vec3(207.6f, 0.f, 84.f), 1.5f, 0.5f, L"Ratau", L"추종자들은 그대를 위해 자원을 수집하오. 이들에게 명령해\n 목재나 돌을 수집하도록 하시오."},
		{_vec3(207.6f, 0.f, 84.f), 0.75f, 0.5f, L"Ratau", L"Ratau_Outro", ADV_EVENT, 0.f, L"Ratau.Done"}
	};

	CCutSceneMgr::GetInstance()->Register_CutScene(tDungeonScene);

	tDungeonScene.strName = L"Gate_00";
	tDungeonScene.vecSteps =
	{
		{_vec3(0.f, 0.f, 18.f), 0.75f, 0.5f, L"", L"", ADV_TIMED, 1.f},
		{_vec3(0.f, 0.f, 18.f), 0.75f, 0.5f, L"Cam", L"Shake_Village", ADV_TIMED, 2.f},
		{_vec3(-18.8f, 0.f, 41.8f), 1.f, 0.5f, L"", L"", ADV_TIMED, 3.f},
	};

	CCutSceneMgr::GetInstance()->Register_CutScene(tDungeonScene);

	CEffectMgr::GetInstance()->Ready_EffectMgr(m_pGraphicDev);

	CSoundMgr::GetInstance()->PlayBGM(L"02.Village.mp3", 0.1f);

	return S_OK;
}

_int CVillage::Update_Scene(const _float& fTimeDelta)
{
	Engine::CTransform* pPlayerTransform = CPersistentMgr::GetInstance()->Get_PlayerTransform();
	if (pPlayerTransform)
	{
		_vec3 vPlayerPos;
		pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
		Engine::CLightMgr::GetInstance()->Update_PointLights(vPlayerPos);
	}

	Key_Input_Village();

	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (m_bLeshyDungeonFlag)
	{
		Engine::CScene* pLoading = CLoading::Create(m_pGraphicDev, LOADING_REALDUNGEON);

		if (nullptr == pLoading)
			return NOEVENT;

		CCollisionMgr::GetInstance()->Reset_For_SceneChange();
		CTileMgr::GetInstance()->Reset_For_SceneChange();
		CSoundMgr::GetInstance()->StopAll();
		CLightMgr::GetInstance()->DestroyInstance();
		CPersistentMgr::GetInstance()->Get_Player()->Set_Village(false);
		CPersistentMgr::GetInstance()->Get_Gauge()->Set_GaugeState(Gauge::GS_PASSION);

		if (FAILED(CManagement::GetInstance()->Set_Scene(pLoading)))
		{
			MSG_BOX("Stage Scene Failed");
			return NOEVENT;
		}
	}
	if (m_bKnuckleBoneFlag)
	{
		Engine::CScene* pLoading = CLoading::Create(m_pGraphicDev, LOADING_KNUCKLEBONE);

		if (nullptr == pLoading)
			return NOEVENT;

		// 매니저 상태 보관
		CTileMgr::GetInstance()->Push_State();
		CCollisionMgr::GetInstance()->Push_State();
		CLightMgr::GetInstance()->Push_State();
		CSoundMgr::GetInstance()->StopAll();

		// 마을 씬 보관
		CPersistentMgr::GetInstance()->Set_Village(this);

		if (FAILED(CManagement::GetInstance()->Set_Scene(pLoading)))
		{
			MSG_BOX("Stage Scene Failed");
			return NOEVENT;
		}
		m_bKnuckleBoneFlag = false;
	}

	return iExit;
}

void CVillage::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CVillage::Render_Scene()
{
}

HRESULT CVillage::Ready_Environment_Layer(const _tchar* pLayerTag)
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



	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CVillage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;


	// Map Load
	Engine::MAPDATA mapData;
	if (SUCCEEDED(Engine::CMapLoader::GetInstance()->LoadMapA(
		"../Bin/Resource/Maps/MapData/Village.txt", mapData)))
	{
		// �� �������� skyType���� SkyBox ����
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
				//CPersistentMgr::GetInstance()->Get_Player()->Set_Pos(_vec3(spawn.x * 0.8f, -0.95f, spawn.z * 0.8f)); // 실제 스폰 지점
				CPersistentMgr::GetInstance()->Get_Player()->Set_Pos(_vec3(199.8f, -0.95f, 35.f));	// 디버그용
				CPersistentMgr::GetInstance()->Get_Player()->Set_Village(true);
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
			else if (obj.category == "BreakableRock")
			{
				//pGameObject = CBreakableRock::Create(m_pGraphicDev, obj, m_pMessageChannel);
				//if (pGameObject)
				//	pLayer->Add_GameObject(L"BreakableRock", pGameObject);
			}
			else if (obj.category == "BreakableTree")
			{
				//pGameObject = CBreakableTree::Create(m_pGraphicDev, obj, m_pMessageChannel);
				//if (pGameObject)
				//	pLayer->Add_GameObject(L"BreakableTree", pGameObject);
			}
			else
			{
				pGameObject = CMapObject::Create(m_pGraphicDev, obj);
				if (pGameObject)
					pLayer->Add_GameObject(L"MapObject", pGameObject);
			}
		}
		// Process Lights - Point Light ����
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
		// �� �ε� ���� �� �⺻ SkyBox (Day) ����
		pGameObject = CMySkyBox::Create(m_pGraphicDev, 0);
		if (pGameObject)
			pLayer->Add_GameObject(L"SkyBox", pGameObject);
	}

	// 디버그용

	for (int i = 0; i < 3; ++i)
	{
		pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower1Texture");
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
			return E_FAIL;

		pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower2Texture");
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
			return E_FAIL;

		pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower3Texture");
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
			return E_FAIL;

		pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower4Texture");
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
			return E_FAIL;

		pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower5Texture");
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
			return E_FAIL;
	}

	// Village 지형물
	for (int i = 0; i < 12; ++i)
	{
		OBJECTDATA tObjData1 = {"BreakableRock",						// 카테고리
								0,										// 텍스처인덱스
								199.8f + Get_Rand_Float(-30.f, 30.f),	// x
								-0.5f,									// y
								35.f + Get_Rand_Float(-30.f, 30.f),		// z
								5.f,									// 스케일
								0 };									// Standing or Floor

		pGameObject = CBreakableRock::Create(m_pGraphicDev, tObjData1, m_pMessageChannel);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		if (FAILED(pLayer->Add_GameObject(L"Breakable", pGameObject)))
			return E_FAIL;

		OBJECTDATA tObjData2 = {"BreakableTree",						// 카테고리
								0,										// 텍스처인덱스
								199.8f + Get_Rand_Float(-30.f, 30.f),	// x
								2.5f,									// y
								35.f + Get_Rand_Float(-30.f, 30.f),		// z
								10.f,									// 스케일
								0 };									// Standing or Floor

		pGameObject = CBreakableTree::Create(m_pGraphicDev, tObjData2, m_pMessageChannel);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		if (FAILED(pLayer->Add_GameObject(L"Breakable", pGameObject)))
			return E_FAIL;
	}

	pGameObject = CBuilding::Create(m_pGraphicDev, m_pMessageChannel, _vec3(199.8f + 10.f, -0.95f, 35.f - 10.f), CBuilding::BT_KNUCKLEBONE);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"Building", pGameObject)))
		return E_FAIL;

	pGameObject = CBuilding::Create(m_pGraphicDev, m_pMessageChannel, _vec3(199.8f - 10.f, -0.95f, 35.f - 10.f), CBuilding::BT_COOK);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"Building", pGameObject)))
		return E_FAIL;
	// Village 지형물

	pGameObject = CRatau::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ 207.6f, 0.f, 84.f });

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	_vec3 vTriggerPos, vTriggerHalfSize;
	vTriggerPos = { 202.6f, 0.f, 84.f };
	vTriggerHalfSize = { 8.f, 5.f, 2.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"Ratau_02", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	vTriggerPos = { -18.8F, 0.f, 41.8f };
	vTriggerHalfSize = { 5.f, 5.f, 5.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_SCENE, L"Real_Dungeon", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	vTriggerPos = { 0.f, 0.f, 18.f };
	vTriggerHalfSize = { 3.f, 3.f, 3.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"Gate_00", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CVillage::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	//----------------------------- 플레이어 UI -----------------------------
	pGameObject = CPersistentMgr::GetInstance()->Get_Gauge();

	if (nullptr == pGameObject)
		return E_FAIL;

	CPersistentMgr::GetInstance()->Get_Gauge()->Set_GaugeState(Gauge::GS_FAITH);

	if (FAILED(pLayer->Add_GameObject(L"Gauge", pGameObject)))
		return E_FAIL;
	pGameObject->AddRef();
	//----------------------------- 플레이어 UI -----------------------------

	pGameObject = CFade::Create(m_pGraphicDev, m_pMessageChannel);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"Fade", pGameObject)))
		return E_FAIL;

	pGameObject = m_pCookingUI = CCookingUIController::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"CookingUIController", pGameObject)))
		return E_FAIL;

	pGameObject = CNPCCommandUI::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"CNPCCommandUI", pGameObject)))
		return E_FAIL;

	pGameObject = CFoodReviewUI::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"CFoodReviewUI", pGameObject)))
		return E_FAIL;


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}


HRESULT CVillage::Ready_Light()
{
	D3DLIGHT9	tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	// 따뜻한 노을빛 느낌의 마을 조명
	tLightInfo.Diffuse = D3DXCOLOR(1.0f, 0.85f, 0.7f, 1.0f);   // 부드러운 주황빛 확산광
	tLightInfo.Specular = D3DXCOLOR(1.0f, 0.9f, 0.75f, 1.0f);   // 살짝 더 밝은 반사광
	tLightInfo.Ambient = D3DXCOLOR(0.6f, 0.45f, 0.35f, 1.0f);  // 어두운 갈색빛 주변광


	tLightInfo.Direction = { 1.f, -1.f, 1.f };

	if (FAILED(CLightMgr::GetInstance()->Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;

	return S_OK;
}

void	CVillage::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Obj_Add", m_pMessageChannel->Subscribe(L"Obj.Add", [this](const IMessageChannel::EVENT& Event) {
{
	CGameObject* pGObj = any_cast<CGameObject*>(Event.hmapData.find(L"Obj")->second);

	if (pGObj != nullptr)
	{
		wstring strLayerTag = any_cast<const _tchar*>(Event.hmapData.find(L"LayerTag")->second);
		wstring strObjTag = any_cast<wstring>(Event.hmapData.find(L"ObjTag")->second);
		auto iter = m_mapLayer.find(strLayerTag);

		if (iter != m_mapLayer.end())
			iter->second->Add_GameObject(strObjTag, pGObj);
	}
}
}) });
}

void CVillage::Ready_Event_Village()
{
	m_hmapSubHandles.insert({ L"Trigger.Activate", m_pMessageChannel->Subscribe(L"Trigger.Activate", [this](const IMessageChannel::EVENT& Event)
		{
			auto TIDiter = Event.hmapData.find(L"Trigger_TID");
			if (TIDiter == Event.hmapData.end()) { return; }
			auto TriggetNameiter = Event.hmapData.find(L"Trigger_Name");
			if (TriggetNameiter == Event.hmapData.end()) { return; }
			if (any_cast<Trigger::TRIGGERID>(TIDiter->second) == Trigger::TI_SCENE)
			{
				if (any_cast<wstring>(TriggetNameiter->second) == L"Real_Dungeon")
				{
					m_bLeshyDungeonFlag = true;
				}
			}
			if (any_cast<Trigger::TRIGGERID>(TIDiter->second) == Trigger::TI_KNUCKLE)
			{
				m_bKnuckleBoneFlag = true;
			}
			if (any_cast<Trigger::TRIGGERID>(TIDiter->second) == Trigger::TI_COOKING)
			{
				m_pCookingUI->Set_CookingState(CCookingUIController::CS_SELECT);
				CPersistentMgr::GetInstance()->Get_Player()->Set_Action(true);
				m_bCookingFlag = true;
			}
		}
	) });
}

void CVillage::Key_Input_Village()
{
	if (m_bCookingFlag)
	{
		if (CDInputMgr::GetInstance()->Key_Down(DIK_BACKSPACE))
		{
			if (m_pCookingUI->Get_State() == CCookingUIController::CS_MINIGAME) { return; }

			m_pCookingUI->Set_State(CCookingUIController::CS_COOKINGEND);
			CPersistentMgr::GetInstance()->Get_Player()->Set_Action(false);
			m_bCookingFlag = false;
		}
	}
}

CVillage* CVillage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CVillage* pTest = new CVillage(pGraphicDev);

	if (FAILED(pTest->Ready_Scene()))
	{
		Safe_Release(pTest);
		MSG_BOX("pVillage Create Failed");
		return nullptr;
	}

	return pTest;
}

void CVillage::Free()
{
	CCollisionMgr::GetInstance()->Reset_For_SceneChange();
	CTileMgr::GetInstance()->Reset_For_SceneChange();
	CSoundMgr::GetInstance()->StopAll();
	CLightMgr::GetInstance()->DestroyInstance();
	CScene::Free();
}
