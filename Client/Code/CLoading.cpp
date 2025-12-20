#include "pch.h"
#include "CLoading.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CStage.h"
#include "CManagement.h"
#include "CFontMgr.h"
#include "CTest.h"


CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID)
	: CScene(pGraphicDev), m_pLoading(nullptr), m_eChangeID(ChangeID)
{
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Scene()
{
	//테스트용
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	m_pLoading = CLoadingThread::Create(m_pGraphicDev, m_eChangeID);

	if (nullptr == m_pLoading)
		return E_FAIL;

	/*_matrix matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matIdentity);

	D3DXMatrixOrthoLH(&matIdentity, (float)WINCX, (float)WINCY, 0.f, 1.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matIdentity);*/
	return S_OK;
}

_int CLoading::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (true == m_pLoading->Get_Finish())
	{
		if (GetAsyncKeyState(VK_RETURN))
		{
			Engine::CScene* pScene = nullptr;
			switch (m_pLoading->Get_Loading())
			{
			case LOADING_STAGE:
				pScene = CStage::Create(m_pGraphicDev);

				if (nullptr == pScene)
					return -1;
				break;

			case LOADING_TEST:
				pScene = CTest::Create(m_pGraphicDev);

				if (nullptr == pScene)
					return -1;
				break;

			case LOADING_BOSS:
				break;
			}
			
			if (FAILED(CManagement::GetInstance()->Set_Scene(pScene)))
			{
				MSG_BOX("Stage Scene Failed");
				return -1;
			}
		}
	}

	return iExit;
}

void CLoading::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CLoading::Render_Scene()
{
	// debug 용

	_vec2		vPos{ 100.f, 100.f };

	CFontMgr::GetInstance()->Render_Font(L"Font_Default", m_pLoading->Get_String(), &vPos, D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

}


HRESULT CLoading::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	// BackGround
	pGameObject = CBackGround::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"BackGround", pGameObject)))
		return E_FAIL;


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CLoading::Ready_Prototype()
{

	/*if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev))))
		return E_FAIL;*/

	/*if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LogoTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Logo/jang.jpg", 1))))
		return E_FAIL;*/


	return S_OK;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID)
{
	CLoading* pLoading = new CLoading(pGraphicDev, ChangeID);

	if (FAILED(pLoading->Ready_Scene()))
	{
		Safe_Release(pLoading);
		MSG_BOX("pLoading Create Failed");
		return nullptr;
	}

	return pLoading;
}

void CLoading::Free()
{
	Safe_Release(m_pLoading);

	CScene::Free();
}
