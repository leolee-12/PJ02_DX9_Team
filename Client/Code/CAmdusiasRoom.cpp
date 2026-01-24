#include "pch.h"
#include "CAmdusiasRoom.h"
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
#include "CTriggerPoint.h"
#include "CLoading.h"
#include "CManagement.h"

CAmdusiasRoom::CAmdusiasRoom(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CAmdusiasRoom::~CAmdusiasRoom()
{
}

HRESULT CAmdusiasRoom::Ready_Scene()
{
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	Ready_Light();

	CSoundMgr::GetInstance()->PlayBGM(L"02.Village.mp3", 0.1f);

	return S_OK;
}

_int CAmdusiasRoom::Update_Scene(const _float& fTimeDelta)
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

void CAmdusiasRoom::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CAmdusiasRoom::Render_Scene()
{
}

HRESULT CAmdusiasRoom::Ready_Environment_Layer(const _tchar* pLayerTag)
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

HRESULT CAmdusiasRoom::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	// �÷��̾ �ֻ�ܿ�


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

		for (const auto& spawn : mapData.spawns)
		{
			switch (spawn.type)
			{
			case 0:
				CPersistentMgr::GetInstance()->Get_Player()->Set_Pos(_vec3(spawn.x * 0.8f, -0.95f, spawn.z * 0.8f)); // 실제 스폰 지점
				//CPersistentMgr::GetInstance()->Get_Player()->Set_Pos(_vec3(199.8f, -0.95f, 35.f));	// 디버그용
				CPersistentMgr::GetInstance()->Get_Player()->Set_Village(true);
				pGameObject = CPersistentMgr::GetInstance()->Get_Player();

				if (nullptr == pGameObject)
					return E_FAIL;

				CPersistentMgr::GetInstance()->Get_Player()->Set_MessageChannel(m_pMessageChannel);

				if (FAILED(pLayer->Add_GameObject(L"Player", pGameObject)))
					return E_FAIL;

				pGameObject->AddRef();
				break;
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

	_vec3 vFollowerPos = { 205.f, 0.f, 101.f };

	pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower1Texture", vFollowerPos, CFollower::FOLLOWER_CHEER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	vFollowerPos = { 193.8f, 0.f, 100.7f };
	pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower2Texture", vFollowerPos, CFollower::FOLLOWER_CHEER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	vFollowerPos = { 199.9f, 0.f, 106.f };
	pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower3Texture", vFollowerPos, CFollower::FOLLOWER_CHEER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	vFollowerPos = { 199.9f, 0.f, 95.f };
	pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower4Texture", vFollowerPos, CFollower::FOLLOWER_CHEER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	vFollowerPos = { 196.5f, 0.f, 96.8f };
	pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower5Texture", vFollowerPos, CFollower::FOLLOWER_CHEER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	vFollowerPos = { 204.3f, 0.f, 96.8f};
	pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower2Texture", vFollowerPos, CFollower::FOLLOWER_CHEER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	vFollowerPos = { 203.3f, 0.f, 104.3f };
	pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower4Texture", vFollowerPos, CFollower::FOLLOWER_CHEER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;

	vFollowerPos = { 196.8f, 0.f, 104.4f};
	pGameObject = CFollower::Create(m_pGraphicDev, m_pMessageChannel, L"Proto_Follower1Texture", vFollowerPos, CFollower::FOLLOWER_CHEER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	if (FAILED(pLayer->Add_GameObject(L"NPC", pGameObject)))
		return E_FAIL;


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CAmdusiasRoom::Ready_UI_Layer(const _tchar* pLayerTag)
{
	return S_OK;
}


HRESULT CAmdusiasRoom::Ready_Light()
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

CAmdusiasRoom* CAmdusiasRoom::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAmdusiasRoom* pTest = new CAmdusiasRoom(pGraphicDev);

	if (FAILED(pTest->Ready_Scene()))
	{
		Safe_Release(pTest);
		MSG_BOX("pVillage2 Create Failed");
		return nullptr;
	}

	return pTest;
}

void CAmdusiasRoom::Free()
{
	CScene::Free();
	CCollisionMgr::GetInstance()->Reset_For_SceneChange();
	CLightMgr::GetInstance()->DestroyInstance();
}
