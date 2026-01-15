#include "pch.h"
#include "CFontUI.h"
#include "CFontMgr.h"
#include "CRenderer.h"

CFontUI::CFontUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_bActive(false), m_iCutText(0), m_fCutDelay(0.f), m_bEnd(false)
{
    ZeroMemory(&m_tRenderRect, sizeof(RECT));
}

CFontUI::CFontUI(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
    : CGameObject(pGraphicDev, pMessageChannel), m_bActive(false), m_iCutText(0), m_fCutDelay(0.f), m_bEnd(false)
{
    ZeroMemory(&m_tRenderRect, sizeof(RECT));
}

CFontUI::~CFontUI()
{
}

HRESULT CFontUI::Ready_GameObject()
{
	Ready_Event();

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

		Update_CutText(fTimeDelta);

        CRenderer::GetInstance()->Add_RenderGroup(RENDER_FONT, this);
    }

    return NOEVENT;
}

void CFontUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CFontUI::Render_GameObject()
{
	wstring strDynamicText = m_strText.substr(0, m_iCutText);
	RECT    rcOwnerRect = m_tRenderRect;
	rcOwnerRect.bottom -= _long(m_vScale.y * 0.5f);

	//DT_CENTER | DT_VCENTER
	CFontMgr::GetInstance()->Render_Font(m_strFont.c_str(), m_strRenderOwnerName.c_str(), rcOwnerRect, D3DXCOLOR(0.85f, 0.8f, 0.3f, 1.f), DT_CENTER | DT_VCENTER);
    CFontMgr::GetInstance()->Render_Font(m_strFont.c_str(), strDynamicText.c_str(), m_tRenderRect, m_tFontColor, m_dwFlags);
}

void CFontUI::OnCollision(CGameObject* pObject)
{
}

CFontUI* CFontUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
    CFontUI* pFontUI = new CFontUI(pGraphicDev, pMessageChannel);

    if (FAILED(pFontUI->Ready_GameObject()))
    {
        Safe_Release(pFontUI);
        MSG_BOX("pFontUI Create Failed");
        return nullptr;
    }

    return pFontUI;
}

void CFontUI::Update_CutText(const _float& fTimeDelta)
{
	if (m_bEnd) { return; }

	if (m_fCutDelay > 0.05f)
	{
		++m_iCutText;
		if (m_iCutText > m_strText.length())
		{
			m_iCutText = m_strText.length();

			IMessageChannel::EVENT  DialogueEvent;
			DialogueEvent.strType = L"Dialogue.End";
			m_pMessageChannel->Publish(DialogueEvent);

			m_bEnd = true;
		}
		m_fCutDelay = 0.f;
	}
	else
	{
		m_fCutDelay += fTimeDelta;
	}
}

void CFontUI::Ready_Event()
{
	m_hmapSubHandles.insert({ L"CutScene.Skip", m_pMessageChannel->Subscribe(L"CutScene.Skip", [this](const IMessageChannel::EVENT& Event)
		{
			auto TargetNameiter = Event.hmapData.find(L"TargetName");
			if (TargetNameiter == Event.hmapData.end()) { return; }
			auto Textiter = Event.hmapData.find(L"Text");
			if (Textiter == Event.hmapData.end()) { return; }

			if (any_cast<wstring>(TargetNameiter->second) != m_strOwnerName) { return; }
			if (any_cast<wstring>(Textiter->second) != m_strText) { return; }

			m_iCutText = m_strText.length();
			//m_bEnd = true;
		}
	) });
}

void CFontUI::Free()
{
    CGameObject::Free();
}
