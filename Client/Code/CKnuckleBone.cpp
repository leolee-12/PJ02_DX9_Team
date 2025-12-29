#include "pch.h"
#include "CKnuckleBone.h"
#include "CProtoMgr.h"
#include "CStage.h"
#include "CManagement.h"
#include "CFontMgr.h"
#include "CTest.h"
#include "CKBBack.h"
#include "CKBTutorial.h"
#include "CFontMgr.h"



CKnuckleBone::CKnuckleBone(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CKnuckleBone::~CKnuckleBone()
{
}

HRESULT CKnuckleBone::Ready_Scene()
{
	//테스트용
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;


	return S_OK;
}

_int CKnuckleBone::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	return iExit;
}

void CKnuckleBone::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CKnuckleBone::Render_Scene()
{
	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);

	D3DXCOLOR FontRed = D3DXCOLOR(1.f, 0.1f, 0.1f, 1.f);

	RECT rc = { 0, 0, LONG(WINCX), 100 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans60", L"플레이  방법", rc, FontColor, DT_CENTER | DT_BOTTOM);
	rc.bottom = 150;
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"플레이어의 점수는 모든 주사위의 값을 합한 것입니다.", rc, FontColor, DT_CENTER | DT_BOTTOM);

	rc = { 0, WINCY / 2,  790, (WINCY / 2) + 125 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans40", L"주사위 일치", rc, FontRed, DT_CENTER | DT_BOTTOM);
	rc.bottom += 75;
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"같은 열에 같은 주사위 눈이 나오는 경우,\n그 값을 곱합니다.", rc, FontColor, DT_CENTER | DT_BOTTOM);

	rc = { 505, WINCY / 2,  WINCX, (WINCY / 2) + 125 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans40", L"상대 파괴", rc, FontRed, DT_CENTER | DT_BOTTOM);
	rc.bottom += 75;
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"상대방의 주사위와 같은 값을 맞춰 상대방의\n주사위를 파괴하세요", rc, FontColor, DT_CENTER | DT_BOTTOM);

	rc = { 0, 0, LONG(WINCX), WINCY - 75 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"확인", rc, FontColor, DT_CENTER | DT_BOTTOM);
}


HRESULT CKnuckleBone::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CKnuckleBone::Ready_UI_Layer(const _tchar* pLayerTag)
{

	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CKBBack::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingBack", pGameObject)))
		return E_FAIL;


	pGameObject = CKBTutorial::Create(m_pGraphicDev, 0);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingBack", pGameObject)))
		return E_FAIL;


	pGameObject = CKBTutorial::Create(m_pGraphicDev, 1);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingBack", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

CKnuckleBone* CKnuckleBone::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKnuckleBone* pKnuckleBone = new CKnuckleBone(pGraphicDev);

	if (FAILED(pKnuckleBone->Ready_Scene()))
	{
		Safe_Release(pKnuckleBone);
		MSG_BOX("pKnuckleBone Create Failed");
		return nullptr;
	}

	return pKnuckleBone;
}

void CKnuckleBone::Free()
{
	CScene::Free();
}
