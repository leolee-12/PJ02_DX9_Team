#include "pch.h"
#include "CEditScene.h"

#include "CProtoMgr.h"

CEditScene::CEditScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CEditScene::~CEditScene()
{
}

HRESULT CEditScene::Ready_Scene()
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

_int CEditScene::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	return iExit;
}

void CEditScene::LateUpdate_Scene(const _float& fTimeDelta)
{
}

void CEditScene::Render_Scene()
{
}

// 격자 무늬, 카메라
HRESULT CEditScene::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	return S_OK;
}

// 내부 오브젝트 배치
HRESULT CEditScene::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	return S_OK;
}

// 임구이UI를 여기서 ..? 음 .... 임구이는 내부 임구이 매니저가 관리해야할 것 같은데?
HRESULT CEditScene::Ready_UI_Layer(const _tchar* pLayerTag)
{
	return S_OK;
}

CEditScene* CEditScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEditScene* pEdit = new CEditScene(pGraphicDev);

	if (FAILED(pEdit->Ready_Scene()))
	{
		Safe_Release(pEdit);
		MSG_BOX("pEdit Create Failed");
		return nullptr;
	}

	return pEdit;
}

void CEditScene::Free()
{
	CScene::Free();
}
