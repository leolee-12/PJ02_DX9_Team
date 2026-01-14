#include "pch.h"
#include "CTheGateway.h"
#include "CCollisionMgr.h"

CTheGateway::CTheGateway(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CTheGateway::~CTheGateway()
{
}

HRESULT CTheGateway::Ready_Scene()
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

_int CTheGateway::Update_Scene(const _float& fTimeDelta)
{
	return _int();
}

void CTheGateway::LateUpdate_Scene(const _float& fTimeDelta)
{
}

void CTheGateway::Render_Scene()
{
}

HRESULT CTheGateway::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CTheGateway::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CTheGateway::Ready_UI_Layer(const _tchar* pLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CTheGateway::Ready_Const_Layer()
{
	return E_NOTIMPL;
}

HRESULT CTheGateway::Ready_Light()
{
	return E_NOTIMPL;
}

CTheGateway* CTheGateway::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return nullptr;
}

void CTheGateway::Free()
{
}
