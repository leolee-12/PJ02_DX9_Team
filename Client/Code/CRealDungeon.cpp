#include "pch.h"
#include "CRealDungeon.h"

CRealDungeon::CRealDungeon(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CRealDungeon::~CRealDungeon()
{
}

HRESULT CRealDungeon::Ready_Scene()
{
    return E_NOTIMPL;
}

_int CRealDungeon::Update_Scene(const _float& fTimeDelta)
{
    return _int();
}

void CRealDungeon::LateUpdate_Scene(const _float& fTimeDelta)
{
}

void CRealDungeon::Render_Scene()
{
}

HRESULT CRealDungeon::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CRealDungeon::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CRealDungeon::Ready_UI_Layer(const _tchar* pLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CRealDungeon::Ready_Const_Layer()
{
    return E_NOTIMPL;
}

HRESULT CRealDungeon::Ready_Light()
{
    return E_NOTIMPL;
}

CRealDungeon* CRealDungeon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return nullptr;
}

void CRealDungeon::Free()
{
}
