#include "pch.h"
#include "CTutorial.h"

CTutorial::CTutorial(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CTutorial::~CTutorial()
{
}

HRESULT CTutorial::Ready_Scene()
{
	return E_NOTIMPL;
}

_int CTutorial::Update_Scene(const _float& fTimeDelta)
{
	return _int();
}

void CTutorial::LateUpdate_Scene(const _float& fTimeDelta)
{
}

void CTutorial::Render_Scene()
{
}

HRESULT CTutorial::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CTutorial::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CTutorial::Ready_UI_Layer(const _tchar* pLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CTutorial::Ready_Const_Layer()
{
	return E_NOTIMPL;
}

HRESULT CTutorial::Ready_Light()
{
	return E_NOTIMPL;
}

CTutorial* CTutorial::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return nullptr;
}

void CTutorial::Free()
{
}
