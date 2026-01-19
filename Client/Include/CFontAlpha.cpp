#include "pch.h"
#include "CFontAlpha.h"
#include "CFontMgr.h"
#include "CRenderer.h"

CFontAlpha::CFontAlpha(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_bActive(false)
{
	
}

CFontAlpha::CFontAlpha(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
	: CGameObject(pGraphicDev, pMessageChannel), m_bActive(false)
{
	
}

CFontAlpha::~CFontAlpha()
{
}

HRESULT CFontAlpha::Ready_GameObject()
{
	Ready_Event();

	return S_OK;
}

_int CFontAlpha::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bActive)
	{
		Update_Alpha();
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_FONT, this);
	}

	return NOEVENT;
}

void CFontAlpha::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CFontAlpha::Render_GameObject()
{
	RECT rc = { 0,0,WINCX,WINCY };
	CFontMgr::GetInstance()->Render_Font(L"Font_Default80", m_strText.c_str(), rc, m_tFontColor, DT_CENTER | DT_VCENTER);
}

void CFontAlpha::OnCollision(CGameObject* pObject)
{
}

void CFontAlpha::Update_Alpha()
{
	
	if (m_bRevers)
	{
		if (m_tFontColor.a > 0.f)
		{
			m_tFontColor.a -= 0.01f;
		}
		else
		{
			UnActive();
		}
	}
	else {
		if (m_tFontColor.a < 1.f)
		{
			m_tFontColor.a += 0.01f;
		}
		else
		{
			m_bRevers = true;
		}
	}

	m_tFontColor.a = clamp(m_tFontColor.a, 0.f, 1.f);
}

CFontAlpha* CFontAlpha::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CFontAlpha* pFontUI = new CFontAlpha(pGraphicDev, pMessageChannel);

	if (FAILED(pFontUI->Ready_GameObject()))
	{
		Safe_Release(pFontUI);
		MSG_BOX("pFontUI Create Failed");
		return nullptr;
	}

	return pFontUI;
}

void CFontAlpha::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Dialogue", m_pMessageChannel->Subscribe(L"CutScene.Dialogue", [this](const IMessageChannel::EVENT& Event)
	{
		auto CinemaTargetNameiter = Event.hmapData.find(L"CinemaTargetName");
		if (CinemaTargetNameiter == Event.hmapData.end()) { return; }
		auto Dothisiter = Event.hmapData.find(L"Dothis");
		if (Dothisiter == Event.hmapData.end()) { return; }
		if (any_cast<wstring>(CinemaTargetNameiter->second) == L"Font")
		{
			m_strText = any_cast<wstring>(Dothisiter->second);
			Active();
			Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 0.f));
		}

		return;
	}
	) });
}

void CFontAlpha::Free()
{
	CGameObject::Free();
}
