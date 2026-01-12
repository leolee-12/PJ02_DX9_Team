#include "pch.h"
#include "CMainApp.h"
#include "CLogo.h"
#include "CStage.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CLightMgr.h"
#include "CCollisionMgr.h"
#include "CPersistentMgr.h"
#include "CSoundMgr.h"
#include "CTileMgr.h"
#include "CMapLoader.h"

bool g_bDebug = false;
list<wstring>		g_MapProtoname;

CMainApp::CMainApp() : m_pDeviceClass(nullptr), m_pGraphicDev(nullptr)
, m_pManagementClass(CManagement::GetInstance())
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp()
{
	srand(unsigned(time(NULL)));
	
	if (FAILED(Ready_DefaultSetting(&m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(Ready_Scene(m_pGraphicDev)))
		return E_FAIL;


	CCollisionMgr::GetInstance()->Ready_CollisionMgr();
	CSoundMgr::GetInstance()->Ready_SoundMgr();

	return S_OK;
}

int CMainApp::Update_MainApp(const float& fTimeDelta)
{
	m_pManagementClass->Update_Scene(fTimeDelta);
	CDInputMgr::GetInstance()->Update_InputDev();

	// 타일 매니저 업데이트
	CTileMgr::GetInstance()->Update(fTimeDelta);

	// _ulong dwDst = 0;

	//if (dwDst = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Z))
	//{
	//	int a = 0;
	//}

	return 0;
}

void CMainApp::LateUpdate_MainApp(const float& fTimeDelta)
{
	m_pManagementClass->LateUpdate_Scene(fTimeDelta);

	// 타일 매니저 레이트 업데이트
	CTileMgr::GetInstance()->LateUpdate(fTimeDelta);

	CCollisionMgr::GetInstance()->Check_Collisions(fTimeDelta);
	CSoundMgr::GetInstance()->Update();
}

void CMainApp::Render_MainApp()
{
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	m_pManagementClass->Render_Scene(m_pGraphicDev);

	m_pDeviceClass->Render_End();
}

HRESULT CMainApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	if (FAILED(CGraphicDev::GetInstance()->Ready_GraphicDev(g_hWnd, MODE_WIN,
		WINCX, WINCY, &m_pDeviceClass)))
	{
		MSG_BOX("GraphicDev Ready Failed");
		return E_FAIL;
	}

	m_pDeviceClass->AddRef();

	(*ppGraphicDev) = m_pDeviceClass->Get_GraphicDev();
	(*ppGraphicDev)->AddRef();


	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

	// DInputMgr

	if (FAILED(CDInputMgr::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;


	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	Ready_Font();


	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	Engine::CScene* pLogo = CLogo::Create(pGraphicDev);

	if (nullptr == pLogo)
		return E_FAIL;

	if (FAILED(m_pManagementClass->Set_Scene(pLogo)))
	{
		Safe_Release(pLogo);
		MSG_BOX("Logo Setting Failed");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Default", L"Malgun Gothic", 0, 20, FW_HEAVY)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Lapture20", L"LaptureDisplay", 0, 20, FW_THIN)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Lapture30", L"LaptureDisplay", 0, 30, FW_THIN)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Lapture40", L"LaptureDisplay", 0, 40, FW_THIN)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Lapture60", L"LaptureDisplay", 0, 60, FW_THIN)))
		return E_FAIL;


	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_NotoSans30", L"Noto Sans KR Regular", 0, 30, FW_REGULAR)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_NotoSans40", L"Noto Sans KR Regular", 0, 40, FW_REGULAR)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_NotoSans60", L"Noto Sans KR Regular", 0, 60, FW_REGULAR)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_NotoSans80", L"Noto Sans KR Regular", 0, 80, FW_REGULAR)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_NotoSans100", L"Noto Sans KR Regular", 0, 100, FW_REGULAR)))
		return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Ready_MainApp()))
	{
		Safe_Release(pMainApp);
		MSG_BOX("MainApp Create Failed");
		return nullptr;
	}


	return pMainApp;
}

void CMainApp::Free()
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);

	CMapLoader::DestroyInstance();
	CTileMgr::DestroyInstance();
	CPersistentMgr::DestroyInstance();
	CLightMgr::DestroyInstance();
	CFontMgr::DestroyInstance();
	CDInputMgr::DestroyInstance();
	CRenderer::DestroyInstance();
	CProtoMgr::DestroyInstance();
	CFrameMgr::DestroyInstance();
	CTimerMgr::DestroyInstance();
	CManagement::DestroyInstance();
	CCollisionMgr::DestroyInstance();
	CSoundMgr::DestroyInstance();
	m_pDeviceClass->DestroyInstance();
	g_MapProtoname.clear();
}
