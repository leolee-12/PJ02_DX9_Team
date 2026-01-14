#include "pch.h"
#include "CVillage.h"

CVillage::CVillage(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CVillage::~CVillage()
{
}

HRESULT CVillage::Ready_Scene()
{
    return E_NOTIMPL;
}

_int CVillage::Update_Scene(const _float& fTimeDelta)
{
    return _int();
}

void CVillage::LateUpdate_Scene(const _float& fTimeDelta)
{
}

void CVillage::Render_Scene()
{
}

HRESULT CVillage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CVillage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CVillage::Ready_UI_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CVillage::Ready_Const_Layer()
{
    return E_NOTIMPL;
}

HRESULT CVillage::Ready_Light()
{
    return E_NOTIMPL;
}

CVillage* CVillage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return nullptr;
}

void CVillage::Free()
{
}
