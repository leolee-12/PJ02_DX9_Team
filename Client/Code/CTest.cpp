#include "pch.h"
#include "CTest.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CCollisionMgr.h"
#include "CDynamicCamera.h"
#include "CSkyBox.h"
#include "CPersistentMgr.h"
#include "CDungeonBack.h"
#include "CDInputMgr.h"
#include "CDungeonIcon.h"
#include "CLightMgr.h"
#include "CDungeonLine.h"
#include "CSoundMgr.h"
#include "CMainCamera.h"
#include "CMonsterN1.h"
#include "CItem.h"
#include "CMonsterN3.h"
#include "PlayerUI.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CTerrain.h"
#include <CMonsterB1.h>
#include "CTarotCard.h"

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

	Ready_Light();

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

	pGameObject = CMainCamera::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"MainCamera", pGameObject)))
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

	// Player
	pGameObject = CPersistentMgr::GetInstance()->Get_Player();

	if (nullptr == pGameObject)
		return E_FAIL;

	CPersistentMgr::GetInstance()->Get_Player()->Set_MessageChannel(m_pMessageChannel);

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

	_float fHalfwincx = _float(WINCX / 2);
	_float fHalfwincy = _float(WINCY / 2);

	_vec3 vPos = { fHalfwincx - (fHalfwincx * 0.6f), fHalfwincy, 0.01f };

	pGameObject = CTarotCard::Create(m_pGraphicDev, m_pMessageChannel, vPos, CTarotCard::TCT_HEART);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"TarotCard", pGameObject)))
		return E_FAIL;

	vPos = { fHalfwincx + (fHalfwincx * 0.3f), fHalfwincy, 0.01f };

	pGameObject = CTarotCard::Create(m_pGraphicDev, m_pMessageChannel, vPos, CTarotCard::TCT_SPEED);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"TarotCard", pGameObject)))
		return E_FAIL;


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CTest::Ready_Light()
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
