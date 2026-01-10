#include "pch.h"
#include "CLeshyRoom.h"

CLeshyRoom::CLeshyRoom(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CLeshyRoom::~CLeshyRoom()
{
}

HRESULT CLeshyRoom::Ready_Scene()
{
    return E_NOTIMPL;
}

_int CLeshyRoom::Update_Scene(const _float& fTimeDelta)
{
    return _int();
}

void CLeshyRoom::LateUpdate_Scene(const _float& fTimeDelta)
{
}

void CLeshyRoom::Render_Scene()
{
}

HRESULT CLeshyRoom::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CLeshyRoom::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CLeshyRoom::Ready_UI_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CLeshyRoom::Ready_Const_Layer()
{
    return E_NOTIMPL;
}

HRESULT CLeshyRoom::Ready_Light()
{
    return E_NOTIMPL;
}

CLeshyRoom* CLeshyRoom::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return nullptr;
}

void CLeshyRoom::Free()
{
}
