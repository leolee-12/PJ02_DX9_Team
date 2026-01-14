#include "pch.h"
#include "CFontUI.h"
#include "CFontMgr.h"
#include "CRenderer.h"

CFontUI::CFontUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_bActive(false)
{
    ZeroMemory(&m_tRenderRect, sizeof(RECT));
}

CFontUI::CFontUI(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
    : CGameObject(pGraphicDev, pMessageChannel), m_bActive(false)
{
    ZeroMemory(&m_tRenderRect, sizeof(RECT));
}

CFontUI::~CFontUI()
{
}

HRESULT CFontUI::Ready_GameObject()
{
    return S_OK;
}

_int CFontUI::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bActive)
    {
        _matrix matView, matProj;

        m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
        m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

        _vec3 vViewPos, vndcPos, vScreenPos;
        D3DXVec3TransformCoord(&vViewPos, &m_vWorldPos, &matView);
        D3DXVec3TransformCoord(&vndcPos, &vViewPos, &matProj);

        vScreenPos.x = (vndcPos.x * 0.5f + 0.5f) * _float(WINCX);
        vScreenPos.y = (-vndcPos.y * 0.5f + 0.5f) * _float(WINCY);

        m_tRenderRect =
        {
            _long(vScreenPos.x - m_vScale.x * 0.5f),
            _long(vScreenPos.y - m_vScale.y * 0.5f),
            _long(vScreenPos.x + m_vScale.x * 0.5f),
            _long(vScreenPos.y + m_vScale.y * 0.5f)
        };

        CRenderer::GetInstance()->Add_RenderGroup(RENDER_FONT, this);
    }

    return NOEVENT;
}

void CFontUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CFontUI::Render_GameObject()
{
    CFontMgr::GetInstance()->Render_Font(m_strFont.c_str(), m_strText.c_str(), m_tRenderRect, m_tFontColor, m_dwFlags);
}

void CFontUI::OnCollision(CGameObject* pObject)
{
}

CFontUI* CFontUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CFontUI* pFontUI = new CFontUI(pGraphicDev);

    if (FAILED(pFontUI->Ready_GameObject()))
    {
        Safe_Release(pFontUI);
        MSG_BOX("pFontUI Create Failed");
        return nullptr;
    }

    return pFontUI;
}

void CFontUI::Free()
{
    CGameObject::Free();
}