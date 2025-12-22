#include "CUi.h"

CUi::CUi(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CUi::~CUi()
{
}

HRESULT CUi::Ready_GameObject()
{
    return S_OK;
}

_int CUi::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CUi::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CUi::Render_GameObject()
{
    CGameObject::Render_GameObject();
}

void CUi::OnCollision(CGameObject* pObject)
{
}

void CUi::Compute_ViewDepth_Ortho(const _vec3* pPos)
{
    m_fDepth = pPos->z;
}

void CUi::Free()
{
    CGameObject::Free();
}