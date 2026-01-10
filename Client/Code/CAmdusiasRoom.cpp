#include "pch.h"
#include "CAmdusiasRoom.h"

CAmdusiasRoom::CAmdusiasRoom(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CAmdusiasRoom::~CAmdusiasRoom()
{
}

HRESULT CAmdusiasRoom::Ready_Scene()
{
    return E_NOTIMPL;
}

_int CAmdusiasRoom::Update_Scene(const _float& fTimeDelta)
{
    return _int();
}

void CAmdusiasRoom::LateUpdate_Scene(const _float& fTimeDelta)
{
}

void CAmdusiasRoom::Render_Scene()
{
}

HRESULT CAmdusiasRoom::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CAmdusiasRoom::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CAmdusiasRoom::Ready_UI_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CAmdusiasRoom::Ready_Const_Layer()
{
    return E_NOTIMPL;
}

HRESULT CAmdusiasRoom::Ready_Light()
{
    return E_NOTIMPL;
}

CAmdusiasRoom* CAmdusiasRoom::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return nullptr;
}

void CAmdusiasRoom::Free()
{
}
