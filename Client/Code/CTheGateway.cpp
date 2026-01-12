#include "pch.h"
#include "CTheGateway.h"

CTheGateway::CTheGateway(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CTheGateway::~CTheGateway()
{
}

HRESULT CTheGateway::Ready_Scene()
{
	return E_NOTIMPL;
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
