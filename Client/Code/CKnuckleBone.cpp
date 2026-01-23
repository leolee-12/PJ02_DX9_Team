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
#include "CKBBoardSlot.h"
#include "CLoading.h"
#include "CKBBoardBack.h"
#include "CKBDiceBox.h"
#include "CPersistentMgr.h"
#include "CKBBack2.h"
#include "CKBCharPlayer.h"
#include "CKBCharNPC.h"



CKnuckleBone::CKnuckleBone(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev), m_eCurKBState(KB_END), m_ePreKBState(KB_END), m_eTitleOption(KBT_PLAY)
	, m_pTitleTab(nullptr), m_pCurDice(nullptr)
	, m_eMainState(MS_END), m_iSelectedCol(1), m_iCurTurn(0)
	, m_fNPCThinkTime(0.f)
	, m_fResultTime(0.f), m_iWinner(-1)
	, m_fShowTime(0.f)
	, m_iLastPlacedCol(0)
	, m_fEnterDelay(0.f)
	, m_bEnterDone(false)
{
	ZeroMemory(m_iBoard, sizeof(m_iBoard));
	ZeroMemory(m_pBoardDice, sizeof(m_pBoardDice));
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
	Debug_Key_Input_KB();
	State_machine();

	// 메인 게임 로직 업데이트
	_bool bSceneChange = false;
	if (m_eCurKBState == KB_MAIN)
	{
		bSceneChange = Update_MainGame(fTimeDelta);
	}

	map<wstring, CLayer*>::iterator iter;
	iter = m_mapLayer.find(L"Environment_Layer");
	if (iter == m_mapLayer.end()) { return NOEVENT; }
	iter->second->Update_Layer(fTimeDelta);

	iter = m_mapLayer.find(m_strLayerTag);
	if (iter != m_mapLayer.end())
	{
		iter->second->Update_Layer(fTimeDelta);
	}

	// 씬 전환 (Update 최하단에서 수행)
	if (bSceneChange)
	{
		// 마을 재진입 플래그 설정
		CPersistentMgr::GetInstance()->Set_VillageReentry(true);
		CManagement::GetInstance()->Set_Scene(CLoading::Create(m_pGraphicDev, LOADING_VILLAGE));
		return NOEVENT;
	}

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
		Render_Font_Main();
		Render_Font_Trun();
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

	/*pGameObject = CKBBack::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBBack", pGameObject)))
		return E_FAIL;*/

	pGameObject = CKBBack2::Create(m_pGraphicDev);

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

	// 보드 슬롯 생성 (18개: 플레이어 9개 + NPC 9개)
	for (_int iOwner = 0; iOwner < 2; ++iOwner)
	{
		for (_int iCol = 0; iCol < 3; ++iCol)
		{
			for (_int iRow = 0; iRow < 3; ++iRow)
			{
				pGameObject = CKBBoardSlot::Create(m_pGraphicDev, iOwner, iCol, iRow);

				if (nullptr == pGameObject)
					return E_FAIL;

				if (FAILED(pLayer->Add_GameObject(L"KBBoardSlot", pGameObject)))
					return E_FAIL;
			}
		}
	}

	// 선택창
	pGameObject = m_pBoardBack = CKBBoardBack::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBBoardBack", pGameObject)))
		return E_FAIL;

	for (_int i = 0; i < 2; ++i) {
		pGameObject = CKBDiceBox::Create(m_pGraphicDev, i);

		if (nullptr == pGameObject)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(L"KBDiceBox", pGameObject)))
			return E_FAIL;
	}

	pGameObject = m_pKBMask = CKBMask::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBMask", pGameObject)))
		return E_FAIL;

	_vec3 vPos = { -420.f, 100.f, 0.2f };

	pGameObject = m_pCharPlayer = CKBCharPlayer::Create(m_pGraphicDev, vPos);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBChar", pGameObject)))
		return E_FAIL;

	vPos = { 420.f, 20.f, 0.2f };

	pGameObject = m_pCharNPC = CKBCharNPC::Create(m_pGraphicDev, vPos);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"KBChar", pGameObject)))
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
			// 게임 시작: 주사위 굴리기
			Ready_MainGame();
			break;
		case KB_END:
			break;
		}

		m_ePreKBState = m_eCurKBState;
	}
}

void CKnuckleBone::Key_Input_KB()
{
	switch (m_eCurKBState)
	{
	case KB_MAIN:
		// 열 선택 중일 때만 입력 처리
		if (m_eMainState == MS_SELECT && m_iCurTurn == 0)	// 플레이어 턴
		{
			// 좌우 키로 열 선택
			if (CDInputMgr::GetInstance()->Key_Down(DIK_LEFT))
			{
				if (m_iSelectedCol > 0) {
					--m_iSelectedCol;
					m_pBoardBack->Move_Left();
				}
			}
			if (CDInputMgr::GetInstance()->Key_Down(DIK_RIGHT))
			{
				if (m_iSelectedCol < 2) {
					++m_iSelectedCol;
					m_pBoardBack->Move_Right();
				}
			}

			// Enter로 주사위 배치
			if (CDInputMgr::GetInstance()->Key_Down(DIK_RETURN))
			{
				// 선택한 열에 빈 칸이 있는지 확인
				_int iRow = Find_EmptyRow(m_iCurTurn, m_iSelectedCol);
				if (iRow >= 0)
				{
					Place_Dice(m_iCurTurn, m_iSelectedCol);
				}
			}
		}
		return;

	case KB_TITLE:
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
		return;

	case KB_TUTO:
		if (CDInputMgr::GetInstance()->Key_Down(DIK_RETURN))
		{
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
		return;

	default:
		return;
	}
}

void CKnuckleBone::Debug_Key_Input_KB()
{
	if (CDInputMgr::GetInstance()->Key_Down(DIK_F2))
	{
		m_iWinner = Get_Winner();
		m_fResultTime = 0.f;
		m_eMainState = MS_RESULT;

		// 캐릭터 애니메이션: 승자/패자
		if (m_iWinner == 0)
		{
			m_pCharPlayer->Set_State(CKBCharPlayer::KBC_WIN_GAME);
			m_pCharNPC->Set_State(CKBCharNPC::KBC_LOSE_GAME);
		}
		else if (m_iWinner == 1)
		{
			m_pCharPlayer->Set_State(CKBCharPlayer::KBC_LOSE_GAME);
			m_pCharNPC->Set_State(CKBCharNPC::KBC_WIN_GAME);
		}
		else
		{
			m_pCharPlayer->Set_State(CKBCharPlayer::KBC_IDLE);
			m_pCharNPC->Set_State(CKBCharNPC::KBC_IDLE);
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
	CSoundMgr::GetInstance()->StopAll();
	CScene::Free();
}

// 메인 게임 로직 업데이트 (true 반환 시 씬 전환됨, 즉시 탈출 필요)
_bool CKnuckleBone::Update_MainGame(const _float& fTimeDelta)
{
	if (nullptr == m_pCurDice) {
		if (m_fEnterDelay >= 1.f)
		{
			map<wstring, CLayer*>::iterator iter = m_mapLayer.find(L"Main_Layer");
			if (iter == m_mapLayer.end()) { return false; }

			CGameObject* pGameObject = nullptr;

			pGameObject = m_pCurDice = CKBDice::Create(m_pGraphicDev, m_iCurTurn);

			if (nullptr == pGameObject)
				return false;

			if (FAILED(iter->second->Add_GameObject(L"KBDice", pGameObject)))
				return false;

			m_bEnterDone = true;

			Start_Roll();
		}
		else {
			m_fEnterDelay += fTimeDelta;
			return false;
		}
	}

	switch (m_eMainState)
	{
	case MS_ROLL:
		// 주사위 굴리기 중 - 주사위가 DS_SHOWING이 되면 선택 단계로
		if (m_pCurDice->GetState() == DS_SHOWING)
		{
			if (m_fShowTime > 0.3f)
			{
				m_eMainState = MS_SELECT;
				if (m_iCurTurn == 0) {
					m_pBoardBack->Set_Select(true);
					m_pBoardBack->Move_Center();
				}
			}
			m_fShowTime += fTimeDelta;
		}
		break;

	case MS_SELECT:
		// NPC 턴이면 AI가 자동 선택 (1초 딜레이)
		if (m_iCurTurn == 1)
		{
			m_fNPCThinkTime += fTimeDelta;

			// 1초 후에 배치
			if (m_fNPCThinkTime >= 1.f)
			{
				_int iCol = NPC_Select_Column();
				if (iCol >= 0)
				{
					Place_Dice(m_iCurTurn, iCol);
				}
				m_fNPCThinkTime = 0.f;
			}
		}
		break;

	case MS_PLACE:
		// 주사위 이동 중 - 이동 완료되면 다음 턴
		if (m_pCurDice->GetState() == DS_IDLE)
		{
			// 배치 완료 후 양쪽 보드의 해당 열 색상 업데이트
			Update_ColumnDiceColors(0, m_iLastPlacedCol);	// 플레이어
			Update_ColumnDiceColors(1, m_iLastPlacedCol);	// NPC

			// 게임 종료 체크
			if (Check_GameEnd())
			{
				m_iWinner = Get_Winner();
				m_fResultTime = 0.f;
				m_eMainState = MS_RESULT;

				// 캐릭터 애니메이션: 승자/패자
				if (m_iWinner == 0)	// 플레이어 승리
				{
					m_pCharPlayer->Set_State(CKBCharPlayer::KBC_WIN_GAME);
					m_pCharNPC->Set_State(CKBCharNPC::KBC_LOSE_GAME);
				}
				else if (m_iWinner == 1)	// NPC 승리
				{
					m_pCharPlayer->Set_State(CKBCharPlayer::KBC_LOSE_GAME);
					m_pCharNPC->Set_State(CKBCharNPC::KBC_WIN_GAME);
				}
				else	// 무승부
				{
					m_pCharPlayer->Set_State(CKBCharPlayer::KBC_IDLE);
					m_pCharNPC->Set_State(CKBCharNPC::KBC_IDLE);
				}
				break;
			}

			// 턴 교대
			Switch_Turn();

			// 새로운 주사위 생성
			CKBDice* pNewDice = CKBDice::Create(m_pGraphicDev, m_iCurTurn);
			if (pNewDice)
			{
				// 레이어에 추가
				auto iter = m_mapLayer.find(L"Main_Layer");
				if (iter != m_mapLayer.end())
				{
					iter->second->Add_GameObject(L"KBDice", pNewDice);
				}
				m_pCurDice = pNewDice;
			}

			// 다음 턴 시작
			Start_Roll();
		}
		break;

	case MS_RESULT:
		// 결과 표시 후 2초 후 씬 전환 요청
		m_fResultTime += fTimeDelta;
		if (m_fResultTime >= 5.f)
		{
			return true;	// 씬 전환 요청
		}
		break;
	}

	return false;
}

// 주사위 굴리기 시작
void CKnuckleBone::Start_Roll()
{
	if (nullptr == m_pCurDice)
		return;

	// 주사위를 굴리기 위치로 이동
	/*_vec3 vRollPos = { -200.f, -200.f, 0.f };
	m_pCurDice->SetPosition(vRollPos);*/

	// 주사위 굴리기 시작
	m_pCurDice->Roll();
	m_eMainState = MS_ROLL;

	// 캐릭터 애니메이션: 현재 턴 캐릭터는 주사위 던지기
	/*if (m_iCurTurn == 0)
	{
		m_pCharPlayer->Set_State(CKBCharPlayer::KBC_PLAY_DICE);
	}
	else
	{
		m_pCharNPC->Set_State(CKBCharNPC::KBC_PLAY_DICE);
	}*/
}

// 주사위 배치
void CKnuckleBone::Place_Dice(_int iOwner, _int iCol)
{
	if (nullptr == m_pCurDice)
		return;

	// 빈 행 찾기
	_int iRow = Find_EmptyRow(iOwner, iCol);
	if (iRow < 0)
		return;		// 해당 열이 꽉 참

	if (m_iCurTurn == 0)
		m_pCharPlayer->Set_State(CKBCharPlayer::KBC_PLAY_DICE);
	else
		m_pCharNPC->Set_State(CKBCharNPC::KBC_PLAY_DICE);

	// 보드 데이터 업데이트
	_int iDiceValue = m_pCurDice->GetValue();
	m_iBoard[iOwner][iCol][iRow] = iDiceValue;

	// 현재 주사위를 보드에 저장
	m_pBoardDice[iOwner][iCol][iRow] = m_pCurDice;

	// 상대방의 같은 열에서 같은 눈 제거
	Remove_OpponentDice(iOwner, iCol, iDiceValue);

	// 주사위 이동 (애니메이션)
	_vec3 vTargetPos = Get_SlotPosition(iOwner, iCol, iRow);
	m_pCurDice->MoveTo(vTargetPos);

	// 배치 열 저장 (이동 완료 후 색상 업데이트용)
	m_iLastPlacedCol = iCol;

	m_eMainState = MS_PLACE;
}

// 빈 행 찾기 (-1이면 꽉 참)
_int CKnuckleBone::Find_EmptyRow(_int iOwner, _int iCol)
{
	// 플레이어: 행 0부터 채움 (위에서 아래로)
	// NPC: 행 0부터 채움
	for (_int iRow = 0; iRow < 3; ++iRow)
	{
		if (m_iBoard[iOwner][iCol][iRow] == 0)
			return iRow;
	}
	return -1;	// 꽉 참
}

// 슬롯 위치 계산 (KBBoard 네임스페이스 상수 사용)
_vec3 CKnuckleBone::Get_SlotPosition(_int iOwner, _int iCol, _int iRow)
{
	using namespace KBBoard;

	_float fX = BOARD_CENTER_X + (iCol - 1) * SLOT_STEP_X;
	_float fY = 0.f;

	if (iOwner == 1)	// NPC
	{
		fY = NPC_BOARD_Y + (iRow) * SLOT_STEP_Y;
	}
	else				// 플레이어
	{
		fY = PLAYER_BOARD_Y - (iRow) * SLOT_STEP_Y;
	}

	return _vec3(fX, fY, 0.05f);
}

// 메인 게임 UI 렌더링
void CKnuckleBone::Render_Font_Main()
{
	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	D3DXCOLOR FontYellow = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);
	D3DXCOLOR FontGreen = D3DXCOLOR(0.2f, 1.f, 0.2f, 1.f);
	D3DXCOLOR FontRed = D3DXCOLOR(1.f, 0.2f, 0.2f, 1.f);

	_int iPlayerScore = Calc_TotalScore(0);
	_int iNPCScore = Calc_TotalScore(1);

	// 게임 결과 표시 (화면 중앙에 크게)
	if (m_eMainState == MS_RESULT)
	{
		wchar_t strResult[64];

		switch (m_iWinner)
		{
		case 0:		// 플레이어 승리
			swprintf_s(strResult, L"어린 양 승리 %d - %d", iPlayerScore, iNPCScore);
			break;
		case 1:		// NPC 승리
			swprintf_s(strResult, L"라타우 승리 %d - %d", iNPCScore, iPlayerScore);
			break;
		default:	// 무승부
			swprintf_s(strResult, L"무승부 %d - %d", iPlayerScore, iNPCScore);
			break;
		}

		RECT rcResult = { 0, (WINCY / 2) - 50, WINCX, (WINCY / 2) + 50 };
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans80", strResult, rcResult, FontColor, DT_CENTER | DT_VCENTER);
	}

	// 선택된 열 표시 (열 선택 중일 때만)
	//if (m_eMainState == MS_SELECT && m_iCurTurn == 0)
	//{
	//	const wchar_t* strCol[] = { L"왼쪽", L"중앙", L"오른쪽" };
	//	RECT rc = { 0, WINCY - 100, WINCX, WINCY - 50 };
	//	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", strCol[m_iSelectedCol], rc, FontYellow, DT_CENTER | DT_TOP);
	//}

	// 점수 표시

	wchar_t szScore[64];

	// 플레이어 총점 (화면 중앙 왼쪽)
	swprintf_s(szScore, L"어린 양\n%d", iPlayerScore);
	RECT rcPlayer = { 0, (WINCY / 2) - 20, (WINCX / 2) - 200, (WINCY / 2) + 100 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szScore, rcPlayer, FontColor, DT_CENTER | DT_BOTTOM);

	// NPC 총점 (화면 중앙 오른쪽)
	swprintf_s(szScore, L"라타우\n%d", iNPCScore);
	RECT rcNPC = { (WINCX / 2) + 200, (WINCY / 2) - 20, WINCX, (WINCY / 2) + 150};
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szScore, rcNPC, FontColor, DT_CENTER | DT_BOTTOM);

	// 열별 점수 표시 (두 보드 사이에 표시)
	using namespace KBBoard;

	// 화면 좌표 변환: 화면 중앙 = (WINCX/2, WINCY/2), UI 좌표는 Y가 반전
	const _int iCenterX = WINCX / 2;
	const _int iCenterY = WINCY / 2;

	for (_int iCol = 0; iCol < 3; ++iCol)
	{
		// 열의 X 위치 (UI 좌표 → 화면 좌표)
		_int iColScreenX = iCenterX + (_int)((iCol - 1) * SLOT_STEP_X);

		// 플레이어 열 점수 (플레이어 보드 위 = 두 보드 사이)
		_int iPlayerColScore = Calc_ColumnScore(0, iCol);
		_int iPlayerScoreY = iCenterY - (_int)(PLAYER_BOARD_Y + SLOT_STEP_Y * 1.2f);
		swprintf_s(szScore, L"%d", iPlayerColScore);
		RECT rcPlayerCol = { iColScreenX - 50, iPlayerScoreY, iColScreenX + 50, iPlayerScoreY + 30 };
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szScore, rcPlayerCol, FontColor, DT_CENTER | DT_TOP);

		// NPC 열 점수 (NPC 보드 아래 = 두 보드 사이)
		_int iNPCColScore = Calc_ColumnScore(1, iCol);
		_int iNPCScoreY = iCenterY - (_int)(NPC_BOARD_Y - SLOT_STEP_Y * 0.8f);
		swprintf_s(szScore, L"%d", iNPCColScore);
		RECT rcNPCCol = { iColScreenX - 50, iNPCScoreY, iColScreenX + 50, iNPCScoreY + 30 };
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szScore, rcNPCCol, FontColor, DT_CENTER | DT_TOP);
	}
}

void CKnuckleBone::Render_Font_Trun()
{
	if (m_bEnterDone)
		return;

	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	RECT rc = { 0, 0, WINCX, WINCY };

	switch (m_iCurTurn)
	{
	case 0:
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans60", L"어린양이 먼저 굴립니다", rc, FontColor, DT_CENTER | DT_VCENTER);
		break;
	case 1:
		CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans60", L"라타우 측이 먼저 굴립니다", rc, FontColor, DT_CENTER | DT_VCENTER);
		break;
	}
}

// 열 점수 계산 (같은 눈이 있으면 배)
_int CKnuckleBone::Calc_ColumnScore(_int iOwner, _int iCol)
{
	// 해당 열의 주사위 눈 값들
	_int iValues[3] = {
		m_iBoard[iOwner][iCol][0],
		m_iBoard[iOwner][iCol][1],
		m_iBoard[iOwner][iCol][2]
	};

	_int iScore = 0;

	// 각 주사위 눈에 대해 같은 눈 개수를 세고 배 적용
	for (_int i = 0; i < 3; ++i)
	{
		if (iValues[i] == 0)
			continue;	// 빈 칸은 스킵

		// 같은 눈 개수 세기
		_int iCount = 0;
		for (_int j = 0; j < 3; ++j)
		{
			if (iValues[j] == iValues[i])
				++iCount;
		}

		// 눈 값 × 같은 눈 개수
		iScore += iValues[i] * iCount;
	}

	return iScore;
}

// 전체 점수 계산 (3열 합산)
_int CKnuckleBone::Calc_TotalScore(_int iOwner)
{
	_int iTotal = 0;

	for (_int iCol = 0; iCol < 3; ++iCol)
	{
		iTotal += Calc_ColumnScore(iOwner, iCol);
	}

	return iTotal;
}

// NPC AI 열 선택
_int CKnuckleBone::NPC_Select_Column()
{
	if (nullptr == m_pCurDice)
		return 1;	// 기본값: 중앙 열

	_int iDiceValue = m_pCurDice->GetValue();

	// 1. 빈 칸이 있는 열 목록 수집
	_int iValidCols[3] = { -1, -1, -1 };
	_int iValidCount = 0;

	for (_int iCol = 0; iCol < 3; ++iCol)
	{
		if (Find_EmptyRow(1, iCol) >= 0)	// NPC(1)의 빈 칸 확인
		{
			iValidCols[iValidCount++] = iCol;
		}
	}

	// 빈 칸이 없으면 -1 반환
	if (iValidCount == 0)
		return -1;

	// 2. 점수 최대화 전략: 같은 눈이 있는 열 찾기
	_int iBestCol = -1;
	_int iBestScore = -1;

	for (_int i = 0; i < iValidCount; ++i)
	{
		_int iCol = iValidCols[i];
		_int iMatchCount = 0;

		// 해당 열에 같은 눈이 몇 개 있는지 확인
		for (_int iRow = 0; iRow < 3; ++iRow)
		{
			if (m_iBoard[1][iCol][iRow] == iDiceValue)
				++iMatchCount;
		}

		// 같은 눈이 있으면 점수가 배가되므로 우선 선택
		_int iScore = iMatchCount * 2 + 1;	// 매칭 많을수록 높은 점수

		if (iScore > iBestScore)
		{
			iBestScore = iScore;
			iBestCol = iCol;
		}
	}

	// 3. 같은 눈이 없으면 빈 칸 있는 열 중 랜덤 선택
	if (iBestCol < 0 || iBestScore <= 1)
	{
		iBestCol = iValidCols[Engine::Get_Rand_Int(0, iValidCount - 1)];
	}

	return iBestCol;
}

// 턴 교대
void CKnuckleBone::Switch_Turn()
{
	if (m_iCurTurn == 0)
	{
		m_pBoardBack->Set_Select(false);
	}
	// 턴 교대: 0 → 1, 1 → 0
	m_iCurTurn = (m_iCurTurn + 1) % 2;

	m_pKBMask->Set_Index(m_iCurTurn);

	// 열 선택 초기화
	m_iSelectedCol = 1;
}

// 상대 주사위 제거 (같은 열, 같은 눈)
void CKnuckleBone::Remove_OpponentDice(_int iOwner, _int iCol, _int iDiceValue)
{
	// 상대방 인덱스
	_int iOpponent = (iOwner == 0) ? 1 : 0;

	_bool bRemoved = false;

	// 상대방의 같은 열에서 같은 눈의 주사위 찾아서 제거
	for (_int iRow = 0; iRow < 3; ++iRow)
	{
		if (m_iBoard[iOpponent][iCol][iRow] == iDiceValue)
		{
			// 보드 데이터 초기화
			m_iBoard[iOpponent][iCol][iRow] = 0;

			// 주사위 객체 삭제
			if (m_pBoardDice[iOpponent][iCol][iRow] != nullptr)
			{
				m_pBoardDice[iOpponent][iCol][iRow]->SetDead();
				m_pBoardDice[iOpponent][iCol][iRow] = nullptr;
			}

			bRemoved = true;
		}
	}

	// 제거된 주사위가 있으면 열 정렬
	if (bRemoved)
	{
		Compact_Column(iOpponent, iCol);

		// 캐릭터 애니메이션: 상대방은 주사위 잃기
		if (iOpponent == 0) {
			m_pCharPlayer->Set_State(CKBCharPlayer::KBC_LOSE_DICE);
			m_pCharNPC->Set_State(CKBCharNPC::KBC_TAKE_DICE);
		}
		else {
			m_pCharNPC->Set_State(CKBCharNPC::KBC_LOSE_DICE);
			m_pCharPlayer->Set_State(CKBCharPlayer::KBC_TAKE_DICE);
		}
	}
}

// 열 내 빈 칸 정렬 (빈 칸을 뒤로 밀어냄)
void CKnuckleBone::Compact_Column(_int iOwner, _int iCol)
{
	// 임시 배열에 빈 칸이 아닌 데이터만 수집
	_int iTempValues[3] = { 0, 0, 0 };
	CKBDice* pTempDice[3] = { nullptr, nullptr, nullptr };
	_int iCount = 0;

	for (_int iRow = 0; iRow < 3; ++iRow)
	{
		if (m_iBoard[iOwner][iCol][iRow] != 0)
		{
			iTempValues[iCount] = m_iBoard[iOwner][iCol][iRow];
			pTempDice[iCount] = m_pBoardDice[iOwner][iCol][iRow];
			++iCount;
		}
	}

	// 보드 데이터 재배치
	for (_int iRow = 0; iRow < 3; ++iRow)
	{
		m_iBoard[iOwner][iCol][iRow] = iTempValues[iRow];
		m_pBoardDice[iOwner][iCol][iRow] = pTempDice[iRow];

		// 주사위 위치 업데이트 (애니메이션 없이 즉시 이동)
		if (pTempDice[iRow] != nullptr)
		{
			_vec3 vNewPos = Get_SlotPosition(iOwner, iCol, iRow);
			pTempDice[iRow]->MoveTo(vNewPos);
		}
	}
}

// 게임 종료 여부 확인 (한 쪽이 9칸 다 채우면 종료)
_bool CKnuckleBone::Check_GameEnd()
{
	for (_int iOwner = 0; iOwner < 2; ++iOwner)
	{
		_int iFilledCount = 0;

		for (_int iCol = 0; iCol < 3; ++iCol)
		{
			for (_int iRow = 0; iRow < 3; ++iRow)
			{
				if (m_iBoard[iOwner][iCol][iRow] != 0)
					++iFilledCount;
			}
		}

		// 9칸 다 채우면 게임 종료
		if (iFilledCount >= 9)
			return true;
	}

	return false;
}

// 승자 반환 (0=플레이어, 1=NPC, -1=무승부)
_int CKnuckleBone::Get_Winner()
{
	_int iPlayerScore = Calc_TotalScore(0);
	_int iNPCScore = Calc_TotalScore(1);

	if (iPlayerScore > iNPCScore)
		return 0;	// 플레이어 승리
	else if (iNPCScore > iPlayerScore)
		return 1;	// NPC 승리
	else
		return -1;	// 무승부
}

HRESULT CKnuckleBone::Ready_MainGame()
{
	m_iCurTurn = Get_Rand_Int(0, 1);
	m_pKBMask->Set_Index(m_iCurTurn);
	m_pKBMask->Show_Mask();
	m_iSelectedCol = 1;

	return S_OK;
}

// 열 주사위 색상 업데이트 (같은 눈 개수에 따라)
void CKnuckleBone::Update_ColumnDiceColors(_int iOwner, _int iCol)
{
	// 해당 열에서 각 눈의 개수 세기
	_int iValueCount[7] = { 0 }; // 인덱스 1~6 사용

	for (_int iRow = 0; iRow < 3; ++iRow)
	{
		_int iValue = m_iBoard[iOwner][iCol][iRow];
		if (iValue > 0 && iValue <= 6)
			++iValueCount[iValue];
	}

	// 개수에 맞는 색으로 설정
	for (_int iRow = 0; iRow < 3; ++iRow)
	{
		if (m_pBoardDice[iOwner][iCol][iRow] == nullptr)
			continue;

		_int iValue = m_iBoard[iOwner][iCol][iRow];
		_int iCount = iValueCount[iValue];

		if (iCount == 3)
			m_pBoardDice[iOwner][iCol][iRow]->Set_ColorBlue();
		else if (iCount == 2)
			m_pBoardDice[iOwner][iCol][iRow]->Set_ColorYellow();
		else
			m_pBoardDice[iOwner][iCol][iRow]->Set_ColorWhite();
	}
}
