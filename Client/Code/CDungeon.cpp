#include "pch.h"
#include "CDungeon.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CDynamicCamera.h"
// #include "CSkyBox.h"  // CMySkyBox占쏙옙 占쏙옙체
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

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	Ready_Light();

	CCollisionMgr::GetInstance()->Ready_CollisionMgr();

	Ready_Event();

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

	if (CDInputMgr::GetInstance()->Key_Down(DIK_RIGHT))
	{
		m_pGauge->Set_GaugeState(Gauge::GS_FAITH);
		m_pGauge->Set_GaugeValue(0.f);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_LEFT))
	{
		m_pGauge->Set_GaugeState(Gauge::GS_PASSION);
		m_pGauge->Set_GaugeValue(0.f);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_DOWN))
	{
		m_pGauge->Add_GaugeValue(-0.5f);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_UP))
	{
		m_pGauge->Add_GaugeValue(0.5f);
	}


	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

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
				pGameObject = CPersistentMgr::GetInstance()->Get_Player();

				if (nullptr == pGameObject)
					return E_FAIL;

				pGameObject->Set_MessageChannel(m_pMessageChannel);

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

	for (_uint i = 0; i < 20; ++i)
	{
		//pGameObject = CMonsterN2::Create(m_pGraphicDev, m_pMessageChannel);
		//
		//NULL_CHECK_RETURN(pGameObject, E_FAIL)
		//
		//	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		//		return E_FAIL;
	}

	pGameObject = CMonsterB1::Create(m_pGraphicDev, m_pMessageChannel);
	
	NULL_CHECK_RETURN(pGameObject, E_FAIL)
	
		if (FAILED(pLayer->Add_GameObject(L"Boss", pGameObject)))
			return E_FAIL;

	//pGameObject = CMonsterB2::Create(m_pGraphicDev, m_pMessageChannel);
	//
	//NULL_CHECK_RETURN(pGameObject, E_FAIL)
	//
	//	if (FAILED(pLayer->Add_GameObject(L"Boss", pGameObject)))
	//		return E_FAIL;


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CDungeon::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

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

	pGameObject = CPlayerHP::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"PlayerHP", pGameObject)))
		return E_FAIL;

	pGameObject = m_pGauge = CGauge::Create(m_pGraphicDev, Gauge::GS_PASSION);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Gauge", pGameObject)))
		return E_FAIL;

	pGameObject = CCookingUIController::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"CookingUIController", pGameObject)))
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

	tLightInfo.Diffuse = D3DXCOLOR(0.6f, 0.3f, 0.85f, 1.f);
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
			wstring strObjTag = any_cast<const _tchar*>(Event.hmapData.find(L"ObjTag")->second);
			auto iter = m_mapLayer.find(strLayerTag);

			if(iter != m_mapLayer.end())
				iter->second->Add_GameObject(strObjTag, pGObj);
		}
	}
	}) });
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
	CScene::Free();
	CCollisionMgr::GetInstance()->Reset_For_SceneChange();
	CLightMgr::GetInstance()->DestroyInstance();
}
