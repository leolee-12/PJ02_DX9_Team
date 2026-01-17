#include "pch.h"
#include "CLogo.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CFontMgr.h"
#include "CTitleBack.h"
#include "CTitleCenter.h"
#include "CTitleLogo.h"
#include "CLightMgr.h"
#include "CLoading.h"
#include "CDInputMgr.h"
#include "CTitleTab.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev), m_pLoading(nullptr)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
	//테스트용
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_Light()))
		return E_FAIL;

	if(FAILED(Ready_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	m_eLogoState = LS_START;

	return S_OK;
}

HRESULT CLogo::Ready_Light()
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

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);
		if (GetAsyncKeyState(VK_RETURN))
		{
			LOADINGID LSTmp = LOADING_END;
			switch (m_eLogoState)
			{
			case LS_START:
				//LSTmp = LOADING_THEGATEWAY;
				LSTmp = LOADING_TUTORIAL;
				break;
			case LS_EDIT:
				// 임시입니다.
				LSTmp = LOADING_THEGATEWAY;
				break;	
			case LS_EXIT:
				DestroyWindow(g_hWnd);
				return EXIT;
			}

			Engine::CScene* pLoading = CLoading::Create(m_pGraphicDev, LSTmp);

			if (nullptr == pLoading)
				return -1;

			if (FAILED(CManagement::GetInstance()->Set_Scene(pLoading)))
			{
				MSG_BOX("Stage Scene Failed");
				return -1;
			}
		}

	return iExit;
}

void CLogo::LateUpdate_Scene(const _float& fTimeDelta)
{
	Key_Input_Logo();
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CLogo::Render_Scene()
{
	// debug 용
	_vec2		vPlay{ _float(WINCX / 2) - 200.f, 320.f };
	_vec2		vEdit{ _float(WINCX / 2) - 200.f, 420.f };
	_vec2		vExit{ _float(WINCX / 2) - 200.f, 520.f };

	//D3DXCOLOR 
	switch (m_eLogoState) {
	case LS_START:
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"플레이", &vPlay, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), DT_CENTER | DT_NOCLIP);
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"개발자 모드", &vEdit, D3DXCOLOR(0.f, 0.f, 0.f, 1.f), DT_CENTER | DT_NOCLIP);
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"종료", &vExit, D3DXCOLOR(0.f, 0.f, 0.f, 1.f), DT_CENTER | DT_NOCLIP);
		break;
	case LS_EDIT:
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"플레이", &vPlay, D3DXCOLOR(0.f, 0.f, 0.f, 1.f), DT_CENTER | DT_NOCLIP);
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"개발자 모드", &vEdit, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), DT_CENTER | DT_NOCLIP);
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"종료", &vExit, D3DXCOLOR(0.f, 0.f, 0.f, 1.f), DT_CENTER | DT_NOCLIP);
		break;
	case LS_EXIT:
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"플레이", &vPlay, D3DXCOLOR(0.f, 0.f, 0.f, 1.f), DT_CENTER | DT_NOCLIP);
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"개발자 모드", &vEdit, D3DXCOLOR(0.f, 0.f, 0.f, 1.f), DT_CENTER | DT_NOCLIP);
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"종료", &vExit, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), DT_CENTER | DT_NOCLIP);
		break;
	}
}

HRESULT CLogo::Ready_UI_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CTitleBack::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"TitleBack", pGameObject)))
		return E_FAIL;

	pGameObject = CTitleCenter::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"TitleCenter", pGameObject)))
		return E_FAIL;

	pGameObject = CTitleLogo::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"TitleLogo", pGameObject)))
		return E_FAIL;

	pGameObject = m_pTitleTab = CTitleTab::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"TitleTab", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CLogo::Ready_Prototype()
{

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MainMenuTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Mainmenu(2)/animation_%04d.png", 48))))
	//	return E_FAIL;

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MainLogoTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Mainmenu/Logo.png", 1))))
	//	return E_FAIL;

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MainTabTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Mainmenu/Tab.png", 1))))
	//	return E_FAIL;

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
	//	return E_FAIL;


	//// 로딩씬 미리 로딩

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LoadingCenterTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Loading/LoadingCenter.png", 1))))
	//	return E_FAIL;

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LoadingFGTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Loading/LoadingFG.png", 1))))
	//	return E_FAIL;

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LoadingLogoTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Loading/LoadingLogo.png", 1))))
	//	return E_FAIL;

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LoadingCircleTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Loading/LoadingCircle.png", 1))))
	//	return E_FAIL;


	return S_OK;
}

void CLogo::Key_Input_Logo()
{
	if (CDInputMgr::GetInstance()->Key_Down(DIK_DOWN))
	{
		if (m_eLogoState < 0 || m_eLogoState >= (LS_END - 1)) { return; }

		_uint uiTmp = (_uint)m_eLogoState;
		++uiTmp;
		m_eLogoState = (LOGOSTATE)uiTmp;

		m_pTitleTab->Move_Down();
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_UP))
	{
		if (m_eLogoState <= 0 || m_eLogoState > LS_END) { return; }

		_uint uiTmp = (_uint)m_eLogoState;
		--uiTmp;
		m_eLogoState = (LOGOSTATE)uiTmp;

		m_pTitleTab->Move_Up();
	}
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo* pLogo = new CLogo(pGraphicDev);

	if (FAILED(pLogo->Ready_Scene()))
	{
		Safe_Release(pLogo);
		MSG_BOX("pLogo Create Failed");
		return nullptr;
	}

	return pLogo;
}

void CLogo::Free()
{
	Safe_Release(m_pLoading);

	CScene::Free();
}
