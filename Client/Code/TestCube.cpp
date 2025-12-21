#include "TestCube.h"

TestCube::TestCube(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
    ZeroMemory(&m_vPos, sizeof(_vec3));
}

TestCube::TestCube(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
    : CGameObject(pGraphicDev, StageChannel)
{
    ZeroMemory(&m_vPos, sizeof(_vec3));
}

TestCube::TestCube(const TestCube& rhs)
    : CGameObject(rhs), m_vPos(rhs.m_vPos)
{
}

TestCube::~TestCube()
{
}

HRESULT TestCube::Ready_GameObject()
{
    return E_NOTIMPL;
}

_int TestCube::Update_GameObject(const _float& fTimeDelta)
{
    return _int();
}

void TestCube::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void TestCube::Render_GameObject()
{
}

HRESULT TestCube::Add_Component()
{
    return E_NOTIMPL;
}

TestCube* TestCube::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
    return nullptr;
}

void TestCube::Free()
{
}
