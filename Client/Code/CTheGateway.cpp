#include "pch.h"
#include "CTheGateway.h"
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
#include "CNarinder.h"
#include "CTriggerPoint.h"
#include "CCutSceneMgr.h"
#include "CFontUIOrtho.h"
#include "CSpeechBubbleOrtho.h"
#include "CSelectionArrow.h"
#include "CLoading.h"
#include "CManagement.h"
#include "CFade.h"

CTheGateway::CTheGateway(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CTheGateway::~CTheGateway()
{
}

HRESULT CTheGateway::Ready_Scene()
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

	CUTSCENE tTutoCutScene;
	tTutoCutScene.strName = L"TheGateway_01";
	tTutoCutScene.vecSteps =
	{
		{_vec3(0.f, 8.f, 68.f * 0.8f), 1.f, 0.5f, L"Narinder", L"가까이 오거라. 두려워 말라, 너는 이미 죽었건만,\n내 아직 너를 필요로 함이라."},
		{_vec3(0.f, 8.f, 68.f * 0.8f), 1.f, 0.5f, L"Narinder", L"저 어리석은 주교들은 죽음으로 나와 너를 가를 수 있다\n생각하였다. 허나 이는 너를 내게 곧바로 보냄이라."},
		{_vec3(0.f, 8.f, 68.f * 0.8f), 1.f, 0.5f, L"Narinder", L"내 너에게 생명을 주나니,\n허나 거기에는 대가가 따름이라!"},
		{_vec3(0.f, 8.f, 68.f * 0.8f), 1.f, 0.5f, L"Narinder", L"내 바라는 것은 오직 하나, 나의 이름을 내세운 교단을\n만드는 것 뿐이니라. 어떻게 생각하느냐?",
		ADV_DIALOGUE, 0.f, L"", vector<wstring>({L"예.", L"물론입니다."})},
		{_vec3(0.f, 8.f, 68.f * 0.8f), 1.f, 0.5f, L"FadeOut", L"", ADV_IMMEDIATE},
		{_vec3(0.f, 0.f, 35.f), 1.5f, 0.5f, L"Player", L"Move_Gateway", ADV_TIMED, 2.f}
		
	};

	CCutSceneMgr::GetInstance()->Register_CutScene(tTutoCutScene);

	CUTSCENE tGateIntro;
	tGateIntro.strName = L"TheGateway_00";
	tGateIntro.vecSteps =
	{
		{_vec3(0.f, 0.f, 0.f), 2.f, 1.0f, L"", L"", ADV_TIMED, 1.0f},
		{_vec3(0.f, 0.f, 0.f), 1.f, 0.25f, L"Player", L"Stop_Crying", ADV_TIMED, 3.0f}
	};

	CCutSceneMgr::GetInstance()->Register_CutScene(tGateIntro);


	CSoundMgr::GetInstance()->PlayBGM(L"01.TheGateway.mp3", 0.1f);

	return S_OK;
}

_int CTheGateway::Update_Scene(const _float& fTimeDelta)
{
	Select_Key_Input();
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
		Engine::CScene* pLoading = CLoading::Create(m_pGraphicDev, LOADING_DUNGEON);

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

void CTheGateway::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CTheGateway::Render_Scene()
{
}

HRESULT CTheGateway::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CMainCamera::Create(m_pGraphicDev, m_pMessageChannel);

	static_cast<CMainCamera*>(pGameObject)->Set_Intro();

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"MainCamera", pGameObject)))
		return E_FAIL;



	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CTheGateway::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	// Map Load
	Engine::MAPDATA mapData;
	if (SUCCEEDED(Engine::CMapLoader::GetInstance()->LoadMapA(
		"../Bin/Resource/Maps/MapData/TheGateway.txt", mapData)))
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
					pGameObject = CNarinder::Create(m_pGraphicDev, m_pMessageChannel, spawn);

					NULL_CHECK_RETURN(pGameObject, E_FAIL)

						if (FAILED(pLayer->Add_GameObject(L"Narinder", pGameObject)))
							return E_FAIL;

					break;
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
		pGameObject = CMySkyBox::Create(m_pGraphicDev, 0);
		if (pGameObject)
			pLayer->Add_GameObject(L"SkyBox", pGameObject);
	}

	_vec3 vTriggerPos, vTriggerHalfSize;
	vTriggerPos = { 0.f, 0.f, 35.f };
	vTriggerHalfSize = { 5.f, 5.f, 5.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"TheGateway_01");

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	vTriggerPos = { 0.f, 0.f, 0.f };
	vTriggerHalfSize = { 5.f, 5.f, 5.f };
	pGameObject = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, vTriggerPos, vTriggerHalfSize, Trigger::TI_STAGING, L"TheGateway_00", true);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"TriggerPoint", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CTheGateway::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	_vec2 vDialoguePos = _vec2(0.f, -250.f);

	pGameObject = m_pLeftSelect = CFontUIOrtho::Create(m_pGraphicDev, m_pMessageChannel);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_pLeftSelect->Set_Pos(_vec2(-125.f, -250.f));
	m_pLeftSelect->Set_Scale(_vec2(250.f, 100.f));

	if (FAILED(pLayer->Add_GameObject(L"Font", pGameObject)))
		return E_FAIL;

	pGameObject = m_pRightSelect = CFontUIOrtho::Create(m_pGraphicDev, m_pMessageChannel);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	m_pRightSelect->Set_Pos(_vec2(125.f, -250.f));
	m_pRightSelect->Set_Scale(_vec2(250.f, 100.f));

	if (FAILED(pLayer->Add_GameObject(L"Font", pGameObject)))
		return E_FAIL;

	pGameObject = m_pSpeechBubble = CSpeechBubbleOrtho::Create(m_pGraphicDev, _vec2(0.f, -250.f), _vec2(500.f, 100.f));

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"Font", pGameObject)))
		return E_FAIL;

	pGameObject = m_pSelectionArrow = CSelectionArrow::Create(m_pGraphicDev, _vec3(0.f, -250.f, 0.01f));

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"Font", pGameObject)))
		return E_FAIL;


	pGameObject = CFade::Create(m_pGraphicDev, m_pMessageChannel);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (FAILED(pLayer->Add_GameObject(L"Fade", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}


HRESULT CTheGateway::Ready_Light()
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

void	CTheGateway::Ready_Event()
{
	m_hmapSubHandles.insert({ L"CutScene.ShowChoice", m_pMessageChannel->Subscribe(L"CutScene.ShowChoice", [this](const IMessageChannel::EVENT& Event) {
		auto SceneNameiter = Event.hmapData.find(L"SceneName");
		if (SceneNameiter == Event.hmapData.end()) { return; }

		if (any_cast<wstring>(SceneNameiter->second) == L"TheGateway_01")
		{
			auto Choiceiter = Event.hmapData.find(L"Choices");
			if (Choiceiter == Event.hmapData.end()) { return; }

			vector<wstring> vecChoiceTex = std::move(any_cast<vector<wstring>>(Choiceiter->second));

			m_pLeftSelect->Set_Text(vecChoiceTex[0].c_str());
			m_pLeftSelect->Set_FontColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
			m_pLeftSelect->Set_Flags(DT_CENTER | DT_VCENTER);

			m_pRightSelect->Set_Text(vecChoiceTex[1].c_str());
			m_pRightSelect->Set_FontColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			m_pRightSelect->Set_Flags(DT_CENTER | DT_VCENTER);

			m_iSelectSlot = 0;

			m_pLeftSelect->Active();
			m_pRightSelect->Active();
			m_pSpeechBubble->Active();
			m_pSelectionArrow->Active();

			m_bShowSelect = true;
		}
	}) });

	m_hmapSubHandles.insert({ L"CutScene.End", m_pMessageChannel->Subscribe(L"CutScene.End", [this](const IMessageChannel::EVENT& Event) {
	if (any_cast<wstring>(Event.hmapData.find(L"SceneName")->second) == L"TheGateway_01")
	{
		m_bSceneChangeFlag = true;
	}
	}) });
}

void CTheGateway::Select_Key_Input()
{
	if (!m_bShowSelect) { return; }

	if (CDInputMgr::GetInstance()->Key_Down(DIK_LEFT))
	{
		if (m_iSelectSlot == 1)
		{
			m_iSelectSlot = 0;
			m_pRightSelect->Set_FontColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			m_pLeftSelect->Set_FontColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
			m_pSelectionArrow->Set_Dir(m_iSelectSlot);
		}
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_RIGHT))
	{
		if (m_iSelectSlot == 0)
		{
			m_iSelectSlot = 1;
			m_pLeftSelect->Set_FontColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			m_pRightSelect->Set_FontColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
			m_pSelectionArrow->Set_Dir(m_iSelectSlot);
		}
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_RETURN))
	{
		IMessageChannel::EVENT tSelectEvent;
		tSelectEvent.strType = L"Choice.Selected";
		m_pLeftSelect->UnActive();
		m_pRightSelect->UnActive();
		m_pSpeechBubble->UnActive();
		m_pSelectionArrow->UnActive();
		m_pMessageChannel->Publish(tSelectEvent);
		m_bShowSelect = false;
	}

}

CTheGateway* CTheGateway::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTheGateway* pTest = new CTheGateway(pGraphicDev);

	if (FAILED(pTest->Ready_Scene()))
	{
		Safe_Release(pTest);
		MSG_BOX("pTheGateway Create Failed");
		return nullptr;
	}

	return pTest;
}

void CTheGateway::Free()
{
	CCollisionMgr::GetInstance()->Reset_For_SceneChange();
	CTileMgr::GetInstance()->Reset_For_SceneChange();
	CSoundMgr::GetInstance()->StopAll();
	CLightMgr::GetInstance()->DestroyInstance();
	CScene::Free();
}
