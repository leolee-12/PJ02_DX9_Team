#include "pch.h"
#include "CTest.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CDynamicCamera.h"
#include "CSkyBox.h"
#include "CPersistentMgr.h"

CTest::CTest(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CTest::~CTest()
{
}

HRESULT CTest::Ready_Scene()
{
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	return S_OK;
}

_int CTest::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	return iExit;
}

void CTest::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CTest::Render_Scene()
{
}

HRESULT CTest::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CSkyBox::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"SkyBox", pGameObject)))
		return E_FAIL;

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

HRESULT CTest::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	// Terrain
	pGameObject = CTerrain::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Terrain", pGameObject)))
		return E_FAIL;

	// Player
	pGameObject = CPersistentMgr::GetInstance()->Get_GlobalObjects(GOBJ_PLAYER);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Player", pGameObject)))
		return E_FAIL;

	pGameObject->AddRef();

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CTest::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CTest::Ready_Const_Layer(CLayer* pConstLayer)
{
	if (nullptr == pConstLayer)
		return E_FAIL;

	auto [iter, inserted] = m_mapLayer.try_emplace(L"Const_Layer");

	if (inserted)
	{
		iter->second = pConstLayer;
		return S_OK;
	}

	Safe_Release(iter->second);
	iter->second = pConstLayer;

	return S_OK;
}

CTest* CTest::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTest* pTest = new CTest(pGraphicDev);

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

void CTest::Free()
{
	CScene::Free();
}
