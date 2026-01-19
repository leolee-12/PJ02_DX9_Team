#include "pch.h"
#include "CStage.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CDynamicCamera.h"
// #include "CSkyBox.h"  // CMySkyBox로 대체
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
#include "CMapLoader.h"
#include "CTileMgr.h"
#include "CMapObject.h"
#include "CGrass.h"
#include "CCollisionMgr.h"
#include <CMonsterB1.h>



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

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	Ready_Light();

	CCollisionMgr::GetInstance()->Ready_CollisionMgr();

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
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

	return iExit;
}

void CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);

	CCollisionMgr::GetInstance()->Check_Collisions(fTimeDelta);
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

	_vec3   vEye{ 0.f, 10.f, -10.f };
	_vec3   vAt{ 0.f, 0.f, 1.f };
	_vec3   vUp{ 0.f, 1.f, 0.f };

	pGameObject = CMainCamera::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"MainCamera", pGameObject)))
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
	}
	else
	{
		// 맵 로드 실패 시 기본 SkyBox (Day) 생성
		pGameObject = CMySkyBox::Create(m_pGraphicDev, 0);
		if (pGameObject)
			pLayer->Add_GameObject(L"SkyBox", pGameObject);
	}

	// Player
	pGameObject = CPersistentMgr::GetInstance()->Get_Player();

	if (nullptr == pGameObject)
		return E_FAIL;

	CPersistentMgr::GetInstance()->Get_Player()->Set_MessageChannel(m_pMessageChannel);

	if (FAILED(pLayer->Add_GameObject(L"Player", pGameObject)))
		return E_FAIL;

	pGameObject->AddRef();

	for (_uint i = 0; i < 1; ++i)
	{
		//pGameObject = CMonsterN1::Create(m_pGraphicDev, m_pMessageChannel);
		//
		//NULL_CHECK_RETURN(pGameObject, E_FAIL)
		//
		//if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		//	return E_FAIL;
		//
		//pGameObject = CMonsterN2::Create(m_pGraphicDev, m_pMessageChannel);
		//
		//NULL_CHECK_RETURN(pGameObject, E_FAIL)
		//
		//	if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
		//		return E_FAIL;

		pGameObject = CMonsterB1::Create(m_pGraphicDev, m_pMessageChannel);

		NULL_CHECK_RETURN(pGameObject, E_FAIL)

			if (FAILED(pLayer->Add_GameObject(L"Monster", pGameObject)))
				return E_FAIL;
	}

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CStage::Ready_UI_Layer(const _tchar* pLayerTag)
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

	pGameObject = CPlayerHP::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"PlayerHP", pGameObject)))
		return E_FAIL;




	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Const_Layer()
{
	return S_OK;
}

HRESULT CStage::Ready_Light()
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

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage* pTest = new CStage(pGraphicDev);

	if (FAILED(pTest->Ready_Scene()))
	{
		Safe_Release(pTest);
		MSG_BOX("pTest Create Failed");
		return nullptr;
	}

	/*if (FAILED(pTest->Ready_Const_Layer(pConstLayer)))
	{
		Safe_Release(pTest);
		MSG_BOX("pTest Create Failed");
		return nullptr;
	}*/

	return pTest;
}

void CStage::Free()
{
	CScene::Free();
	CCollisionMgr::GetInstance()->Reset_For_SceneChange();
	CLightMgr::GetInstance()->DestroyInstance();
}
