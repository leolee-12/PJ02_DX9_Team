#include "pch.h"
#include "CRealDungeon.h"
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
#include "CMonsterB2.h"
#include "CFade.h"
#include "CTriggerPoint.h"
#include "CCutSceneMgr.h"
#include "CFontAlpha.h"
#include "CChest.h"
#include "CEffectMgr.h"

CRealDungeon::CRealDungeon(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CRealDungeon::~CRealDungeon()
{
}

HRESULT CRealDungeon::Ready_Scene()
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

	CUTSCENE tRealDungeonScene;
	tRealDungeonScene.strName = L"Meet_Amdu";
	tRealDungeonScene.vecSteps =
	{
		{_vec3(-145.7f * 0.8f, 1.f, 13.5f * 0.8f), 0.75f, 0.5f, L"", L"", ADV_TIMED, 1.f},
		{_vec3(-145.7f * 0.8f, 1.f, 13.5f * 0.8f), 0.75f, 0.5f, L"Font", L"암두시아스", ADV_IMMEDIATE},
		{_vec3(-145.7f * 0.8f, 1.f, 13.5f * 0.8f), 0.75f, 0.5f, L"Amdu", L"Amdu_Intro", ADV_EVENT, 0.f, L"Amdu.Done"},
		{_vec3(-145.7f * 0.8f, 1.f, 13.5f * 0.8f), 1.5f, 0.5f, L"Amdu", L"Amdu_Intro2", ADV_EVENT, 0.f, L"Amdu.Done"},
		{_vec3(-145.7f * 0.8f, 1.f, 13.5f * 0.8f), 1.5f, 0.5f, L"Sound", L"PlayAmdu", ADV_IMMEDIATE},
	};

	CCutSceneMgr::GetInstance()->Register_CutScene(tRealDungeonScene);

	tRealDungeonScene.strName = L"Meet_Leshy";
	tRealDungeonScene.vecSteps =
	{
		{_vec3(-325.23975f * 0.8f, 4.f, 29.831284f * 0.8f), 0.75f, 0.5f, L"", L"", ADV_TIMED, 1.f},
		{_vec3(-325.23975f * 0.8f, 4.f, 29.831284f * 0.8f), 0.75f, 0.5f, L"Bishop_Leshy", L"Leshy_Enter", ADV_EVENT, 0.f, L"Leshy.Done"},
		{_vec3(-325.23975f * 0.8f, 4.f, 29.831284f * 0.8f), 1.f, 0.5f, L"Bishop_Leshy", L"네가 귀찮아지기 시작했다, 어린 양이여..."},
		{_vec3(-325.23975f * 0.8f, 4.f, 29.831284f * 0.8f), 1.f, 0.5f, L"Bishop_Leshy", L"이 바보같은 가면무도회를 끝낼시간이다!"},
		{_vec3(-325.23975f * 0.8f, 2.f, 29.831284f * 0.8f), 1.2f, 0.25f, L"", L"", ADV_TIMED, 1.f},
		{_vec3(-325.23975f * 0.8f, 2.f, 29.831284f * 0.8f), 0.75f, 0.5f, L"Bishop_Leshy", L"Leshy_Transform", ADV_EVENT, 0.f, L"Leshy.Done"},
		{_vec3(-325.23975f * 0.8f, 2.f, 29.831284f * 0.8f), 0.5f, 0.75f, L"Leshy", L"Leshy_Intro", ADV_IMMEDIATE},
		{_vec3(-325.23975f * 0.8f, 2.f, 29.831284f * 0.8f), 0.5f, 0.75f, L"Font", L"레쉬", ADV_TIMED, 1.f},
		{_vec3(-325.23975f * 0.8f, 2.f, 29.831284f * 0.8f), 0.75f, 0.75f, L"", L"", ADV_TIMED, 1.5f},
		{_vec3(-325.23975f * 0.8f, 2.f, 29.831284f * 0.8f), 0.75f, 1.f, L"Cam", L"Shake", ADV_EVENT, 0.f, L"Leshy.Done"},
		{_vec3(-325.23975f * 0.8f, 2.f, 29.831284f * 0.8f), 0.75f, 1.f, L"Sound", L"PlayLeshy", ADV_IMMEDIATE},
	};

	CCutSceneMgr::GetInstance()->Register_CutScene(tRealDungeonScene);

	CSoundMgr::GetInstance()->PlayBGM(L"05.RealDungeon.mp3", 0.1f);

	return S_OK;
}

_int CRealDungeon::Update_Scene(const _float& fTimeDelta)
{
	Engine::CTransform* pPlayerTransform = CPersistentMgr::GetInstance()->Get_PlayerTransform();
	if (pPlayerTransform)
	{
		_vec3 vPlayerPos;
		pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
		Engine::CLightMgr::GetInstance()->Update_PointLights(vPlayerPos);
	}

	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	return iExit;
}

void CRealDungeon::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CRealDungeon::Render_Scene()
{
}

HRESULT CRealDungeon::Ready_Environment_Layer(const _tchar* pLayerTag)
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

	pGameObject = CChest::Create(m_pGraphicDev, m_pMessageChannel, _vec3(292.f, 0.f, 22.f), 10);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"Chest", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CRealDungeon::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;




	// Map Load
	Engine::MAPDATA mapData;
	if (SUCCEEDED(Engine::CMapLoader::GetInstance()->LoadMapA(
		"../Bin/Resource/Maps/MapData/Real_Dungeon.txt", mapData)))
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
				//CPersistentMgr::GetInstance()->Get_Player()->Set_Pos(_vec3(spawn.x, 0.f, spawn.z));
				CPersistentMgr::GetInstance()->Get_Player()->Set_Pos(_vec3(-88.f, -0.95f, 11.7f)); // 암두방 앞
				//CPersistentMgr::GetInstance()->Get_Player()->Set_Pos(_vec3(-260.f, -0.95f, -5.2f)); // 레쉬방 앞
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
				case 3:
					// Amdusias | 중간보스 |
					pGameObject = CMonsterB1::Create(m_pGraphicDev, m_pMessageChannel, _vec3(spawn.x * 0.8f, 0.f, spawn.z * 0.8f));

					NULL_CHECK_RETURN(pGameObject, E_FAIL)

						if (FAILED(pLayer->Add_GameObject(L"Boss", pGameObject)))
							return E_FAIL;
					break;
				case 4:
					// Rash | 최종보스 |
					pGameObject = CMonsterB2::Create(m_pGraphicDev, m_pMessageChannel, _vec3(spawn.x * 0.8f, 0.f, spawn.z * 0.8f));

					NULL_CHECK_RETURN(pGameObject, E_FAIL)

						if (FAILED(pLayer->Add_GameObject(L"Boss", pGameObject)))
							return E_FAIL;
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

	CBishop_Leshy* pTest;

	pGameObject = pTest = CBishop_Leshy::Create(m_pGraphicDev, m_pMessageChannel, _vec3(-325.23975f * 0.8f, 0.f, 29.831284f * 0.8f));

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	pTest->Set_Wait();

	if (FAILED(pLayer->Add_GameObject(L"Test", pGameObject)))
		return E_FAIL;

	_vec3 vTriggerPos, vTriggerHalfSize;
	vTriggerPos = { -99.F, 0.f, 11.7f };
	vTriggerHalfSize = { 5.f, 5.f, 5.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"Meet_Amdu", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	vTriggerPos = { 12.f, 0.f, 16.6f };
	vTriggerHalfSize = { 2.f, 2.f, 2.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"Meet_Leshy", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	vTriggerPos = { -260.f, 0.f, 2.2f };
	vTriggerHalfSize = { 5.f, 5.f, 2.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"Meet_Leshy", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CRealDungeon::Ready_UI_Layer(const _tchar* pLayerTag)
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

	pGameObject = CFade::Create(m_pGraphicDev, m_pMessageChannel);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"Fade", pGameObject)))
		return E_FAIL;

	pGameObject = CFontAlpha::Create(m_pGraphicDev, m_pMessageChannel);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"Font", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}


HRESULT CRealDungeon::Ready_Light()
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

void CRealDungeon::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Obj_Add", m_pMessageChannel->Subscribe(L"Obj.Add", [this](const IMessageChannel::EVENT& Event) {
	{
		CGameObject* pObj = any_cast<CGameObject*>(Event.hmapData.find(L"Obj")->second);

		if (pObj != nullptr)
		{
			wstring strLayerTag = any_cast<const _tchar*>(Event.hmapData.find(L"LayerTag")->second);
			wstring strObjTag = any_cast<wstring>(Event.hmapData.find(L"ObjTag")->second);
			auto iter = m_mapLayer.find(strLayerTag);

			if (iter != m_mapLayer.end())
				iter->second->Add_GameObject(strObjTag, pObj);
			else
			{
				CLayer* pLayer = CLayer::Create();

				if (nullptr == pLayer)
					return E_FAIL;

				if (FAILED(pLayer->Add_GameObject(strObjTag, pObj)))
					return E_FAIL;

				m_mapLayer.insert({ strLayerTag , pLayer });
			}
		}
	}
	}) });

	m_hmapSubHandles.insert({ L"Summon_Dead", m_pMessageChannel->Subscribe(L"Summon.Dead", [this](const IMessageChannel::EVENT& Event) {
{
	wstring strLayerTag = any_cast<const _tchar*>(Event.hmapData.find(L"LayerTag")->second);

	auto iter = m_mapLayer.find(strLayerTag);

	if (iter != m_mapLayer.end())
		iter->second->Reset_Layer();
}
}) });

	m_hmapSubHandles.insert({ L"Dialogue", m_pMessageChannel->Subscribe(L"CutScene.Dialogue", [this](const IMessageChannel::EVENT& Event)
	{
		auto CinemaTargetNameiter = Event.hmapData.find(L"CinemaTargetName");
		if (CinemaTargetNameiter == Event.hmapData.end()) { return; }
		auto Dothisiter = Event.hmapData.find(L"Dothis");
		if (Dothisiter == Event.hmapData.end()) { return; }
		if (any_cast<wstring>(CinemaTargetNameiter->second) == L"Sound")
		{
			wstring strDothis = any_cast<wstring>(Dothisiter->second);
			if (strDothis == L"BGMStop") {
				CSoundMgr::GetInstance()->StopSound(SOUND_BGM);
				return;
			}
			if (strDothis == L"PlayAmdu") {
				CSoundMgr::GetInstance()->StopSound(SOUND_BGM);
				CSoundMgr::GetInstance()->PlayBGM(L"06.Amdu.mp3", 0.2f);
				return;
			}
			if (strDothis == L"PlayLeshy") {
				CSoundMgr::GetInstance()->StopSound(SOUND_BGM);
				CSoundMgr::GetInstance()->PlayBGM(L"07.Leshy.mp3", 0.2f);
				return;
			}
		}
		if (any_cast<wstring>(CinemaTargetNameiter->second) == L"Scene")
		{
			wstring strDothis = any_cast<wstring>(Dothisiter->second);
			if (strDothis == L"Create_ChestMB") {

				CGameObject* pGameObject = nullptr;

				pGameObject = CChest::Create(m_pGraphicDev, m_pMessageChannel, _vec3(-115.9f, 0.f, 13.5f), 50);

				if (pGameObject == nullptr) { return; }

				auto iter = m_mapLayer.find(L"Environment_Layer");
				if (iter == m_mapLayer.end()) { return; }

				iter->second->Add_GameObject(L"Chest", pGameObject);
				return;
			}
			if (strDothis == L"Create_ChestLB") {

				CGameObject* pGameObject = nullptr;

				pGameObject = CChest::Create(m_pGraphicDev, m_pMessageChannel, _vec3(-260.2f, 0.f, 26.2f), 100);

				if (pGameObject == nullptr) { return; }

				auto iter = m_mapLayer.find(L"Environment_Layer");
				if (iter == m_mapLayer.end()) { return; }

				iter->second->Add_GameObject(L"Chest", pGameObject);
				return;
			}
		}
		return;
	}
	) });

	m_hmapSubHandles.insert({ L"Boss.Dead", m_pMessageChannel->Subscribe(L"Boss.Dead", [this](const IMessageChannel::EVENT& Event)
	{
		auto BossNameiter = Event.hmapData.find(L"BossName");
		if (BossNameiter == Event.hmapData.end()) { return; }
		wstring strDothis = any_cast<wstring>(BossNameiter->second);
		if (strDothis == L"Amdu") {
			CSoundMgr::GetInstance()->StopSound(SOUND_BGM);
			CSoundMgr::GetInstance()->PlayBGM(L"05.RealDungeon.mp3", 0.1f);
			return;
		}
		if (strDothis == L"Leshy") {
			CSoundMgr::GetInstance()->StopSound(SOUND_BGM);
			return;
		}

		return;
	}
	) });
}

CRealDungeon* CRealDungeon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRealDungeon* pTest = new CRealDungeon(pGraphicDev);

	if (FAILED(pTest->Ready_Scene()))
	{
		Safe_Release(pTest);
		MSG_BOX("pRealDungeon Create Failed");
		return nullptr;
	}

	return pTest;
}

void CRealDungeon::Free()
{
	CScene::Free();
	CCollisionMgr::GetInstance()->Reset_For_SceneChange();
	CLightMgr::GetInstance()->DestroyInstance();
}
