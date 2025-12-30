#include "pch.h"
#include "CKnuckleBone.h"
#include "CProtoMgr.h"
#include "CStage.h"
#include "CManagement.h"
#include "CFontMgr.h"
#include "CTest.h"
#include "CKBBack.h"
#include "CKBTutorial.h"
#include "CLightMgr.h"
#include "CKBCenter.h"
#include "CKBTab.h"
#include "CDivider.h"
#include "CKBMask.h"
#include "CDInputMgr.h"
#include "CSoundMgr.h"
#include "CKBSix.h"
#include "CKBDice.h"



CKnuckleBone::CKnuckleBone(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev), m_eCurKBState(KB_END), m_ePreKBState(KB_END), m_eTitleOption(KBT_PLAY)
{
}

CKnuckleBone::~CKnuckleBone()
{
}

HRESULT CKnuckleBone::Ready_Scene()
{
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Title_Layer(L"Title_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Tutorial_Layer(L"Tutorial_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Main_Layer(L"Main_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	m_pTitleTab->Move_Title();


	m_eCurKBState = KB_TITLE;

	CSoundMgr::GetInstance()->PlayBGM(L"KB_BGM.mp3", 0.2f);

	return S_OK;
}

_int CKnuckleBone::Update_Scene(const _float& fTimeDelta)
{
	Key_Input_KB();
	State_machine();

	map<wstring, CLayer*>::iterator iter;
	iter = m_mapLayer.find(L"Environment_Layer");
	if (iter == m_mapLayer.end()) { return NOEVENT; }
	iter->second->Update_Layer(fTimeDelta);

	iter = m_mapLayer.find(m_strLayerTag);
	if (iter == m_mapLayer.end()) { return NOEVENT; }
	return iter->second->Update_Layer(fTimeDelta);


	return NOEVENT;
}

void CKnuckleBone::LateUpdate_Scene(const _float& fTimeDelta)
{
	map<wstring, CLayer*>::iterator iter;
	iter = m_mapLayer.find(L"Environment_Layer");
	if (iter == m_mapLayer.end()) { return; }
	iter->second->LateUpdate_Layer(fTimeDelta);

	iter = m_mapLayer.find(m_strLayerTag);
	if (iter == m_mapLayer.end()) { return; }
	iter->second->LateUpdate_Layer(fTimeDelta);
}

void CKnuckleBone::Render_Scene()
{
	switch (m_eCurKBState)
	{
	case KB_TITLE:
		Render_Font_Title();
		break;
	case KB_TUTO:
		Render_Font_Tutorial();
		break;
	case KB_MAIN:
		break;
	case KB_END:
		break;
	}
}

HRESULT CKnuckleBone::Ready_Tutorial_Layer(const _tchar* pLayerTag)
{

	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CKBTutorial::Create(m_pGraphicDev, 0);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBTutorial", pGameObject)))
		return E_FAIL;


	pGameObject = CKBTutorial::Create(m_pGraphicDev, 1);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBTutorial", pGameObject)))
		return E_FAIL;


	pGameObject = CKBCenter::Create(m_pGraphicDev, D3DXCOLOR(0.7f, 0.f, 0.f, 1.f));

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBCenter", pGameObject)))
		return E_FAIL;

	pGameObject = CKBTab::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBTab", pGameObject)))
		return E_FAIL;

	_vec3 DividerPos = _vec3(-160.f, 290.f, 0.25f);

	pGameObject = CDivider::Create(m_pGraphicDev, 0, DividerPos, 0.7f);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBTab", pGameObject)))
		return E_FAIL;

	DividerPos.x += 320.f;

	pGameObject = CDivider::Create(m_pGraphicDev, 1, DividerPos, 0.7f);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBTab", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CKnuckleBone::Ready_Title_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CKBCenter::Create(m_pGraphicDev, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBCenter", pGameObject)))
		return E_FAIL;

	pGameObject = m_pTitleTab = CKBTab::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBTab", pGameObject)))
		return E_FAIL;

	_vec3 SixPos = { -40.f, 200.f, 0.25f };

	for (_uint i = 0; i < 3; ++i) 
	{
		pGameObject = CKBSix::Create(m_pGraphicDev, SixPos);

		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(L"KBSix", pGameObject)))
			return E_FAIL;

		SixPos.x += 40.f;
	}

	_vec3 DividerPos = _vec3(-100.f, 200.f, 0.25f);

	pGameObject = CDivider::Create(m_pGraphicDev, 0, DividerPos, 0.5f);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBTab", pGameObject)))
		return E_FAIL;

	DividerPos.x += 200.f;

	pGameObject = CDivider::Create(m_pGraphicDev, 1, DividerPos, 0.5f);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBTab", pGameObject)))
		return E_FAIL;


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CKnuckleBone::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CKBBack::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBBack", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CKnuckleBone::Ready_Main_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	_vec3 vDicePos = { -200.f, -200.f, 0.f };
	pGameObject = m_pCurDice = CKBDice::Create(m_pGraphicDev, vDicePos);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBDice", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CKnuckleBone::Ready_Light()
{
	D3DLIGHT9	tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);

	tLightInfo.Direction = { 0.f, 0.f, 1.f };

	if (FAILED(CLightMgr::GetInstance()->Ready_Light(m_pGraphicDev, &tLightInfo, 0)))
		return E_FAIL;


	return S_OK;
}

void CKnuckleBone::Render_Font_Tutorial()
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
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"상대방의 주사위와 같은 값을 맞춰 상대방의\n주사위를 파괴하세요.", rc, FontColor, DT_CENTER | DT_BOTTOM);

	rc = { 0, 0, LONG(WINCX), WINCY - 75 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"확인", rc, FontColor, DT_CENTER | DT_BOTTOM);
}

void CKnuckleBone::Render_Font_Title()
{
	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);

	D3DXCOLOR FontGray = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);

	RECT rc = { 0, 0, LONG(WINCX), 270 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans100", L"너클본", rc, FontColor, DT_CENTER | DT_BOTTOM);
	rc.bottom += 150;
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans40", L"위험과 보상이 있는 주사위 게임", rc, FontColor, DT_CENTER | DT_BOTTOM);

	switch (m_eTitleOption) {
	case KBT_PLAY:
		rc = { 0, 0, LONG(WINCX), WINCY - 75 };
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"종료", rc, FontGray, DT_CENTER | DT_BOTTOM);
		rc.bottom -= 60;
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"플레이 방법", rc, FontGray, DT_CENTER | DT_BOTTOM);
		rc.bottom -= 60;
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"플레이", rc, FontColor, DT_CENTER | DT_BOTTOM);
		break;
	case KBT_HOWTOPLAY:
		rc = { 0, 0, LONG(WINCX), WINCY - 75 };
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"종료", rc, FontGray, DT_CENTER | DT_BOTTOM);
		rc.bottom -= 60;
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"플레이 방법", rc, FontColor, DT_CENTER | DT_BOTTOM);
		rc.bottom -= 60;
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"플레이", rc, FontGray, DT_CENTER | DT_BOTTOM);
		break;
	case KBT_EXIT:
		rc = { 0, 0, LONG(WINCX), WINCY - 75 };
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"종료", rc, FontColor, DT_CENTER | DT_BOTTOM);
		rc.bottom -= 60;
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"플레이 방법", rc, FontGray, DT_CENTER | DT_BOTTOM);
		rc.bottom -= 60;
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"플레이", rc, FontGray, DT_CENTER | DT_BOTTOM);
		break;
	}
}

void CKnuckleBone::State_machine()
{
	if (m_eCurKBState != m_ePreKBState)
	{
		switch (m_eCurKBState)
		{
		case KB_TITLE:
			m_strLayerTag = L"Title_Layer";
			break;
		case KB_TUTO:
			m_strLayerTag = L"Tutorial_Layer";
			break;
		case KB_MAIN:
			m_strLayerTag = L"Main_Layer";
			break;
		case KB_END:
			break;
		}

		m_ePreKBState = m_eCurKBState;
	}
}

void CKnuckleBone::Key_Input_KB()
{
	if (CDInputMgr::GetInstance()->Key_Down(DIK_DOWN))
	{
		if (m_eTitleOption < 0 || m_eTitleOption >= (KBT_END - 1)) { return; }

		_uint uiTmp = (_uint)m_eTitleOption;
		++uiTmp;
		m_eTitleOption = (KBTITLEOPTION)uiTmp;

		m_pTitleTab->Move_Down();
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_UP))
	{
		if (m_eTitleOption <= 0 || m_eTitleOption > KBT_END) { return; }

		_uint uiTmp = (_uint)m_eTitleOption;
		--uiTmp;
		m_eTitleOption = (KBTITLEOPTION)uiTmp;

		m_pTitleTab->Move_Up();
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_RETURN))
	{
		if (m_eCurKBState == KB_MAIN)
		{
			m_pCurDice->MoveTo(_vec3(200.f, 200.f, 0.f));
			return;
		}


		switch (m_eTitleOption)
		{
		case KBT_PLAY:
			m_eCurKBState = KB_MAIN;
			break;
		case KBT_HOWTOPLAY:
			if (m_eCurKBState != KB_TUTO) {
				m_eCurKBState = KB_TUTO;
				break;
			}
			m_eCurKBState = KB_TITLE;
			break;
		case KBT_EXIT:
			DestroyWindow(g_hWnd);
			break;
		}
	}
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
