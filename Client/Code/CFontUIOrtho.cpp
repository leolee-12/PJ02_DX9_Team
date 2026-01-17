#include "pch.h"
#include "CFontUIOrtho.h"
#include "CFontMgr.h"
#include "CRenderer.h"

CFontUIOrtho::CFontUIOrtho(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_bActive(false)
{
	ZeroMemory(&m_tRenderRect, sizeof(RECT));
}

CFontUIOrtho::CFontUIOrtho(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
	: CGameObject(pGraphicDev, pMessageChannel), m_bActive(false)
{
	ZeroMemory(&m_tRenderRect, sizeof(RECT));
}

CFontUIOrtho::~CFontUIOrtho()
{
}

HRESULT CFontUIOrtho::Ready_GameObject()
{
	Ready_Event();

	return S_OK;
}

_int CFontUIOrtho::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bActive)
	{
		_vec2 vScreenPos;
		/*vScreenPos.x = (m_vPos.x * 0.5f + 0.5f) * _float(WINCX);
		vScreenPos.y = (-m_vPos.y * 0.5f + 0.5f) * _float(WINCY);*/

		vScreenPos.x = m_vPos.x + _float(WINCX / 2);
		vScreenPos.y = -m_vPos.y + _float(WINCY / 2);

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

void CFontUIOrtho::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CFontUIOrtho::Render_GameObject()
{
	CFontMgr::GetInstance()->Render_Font(L"Font_Default24", m_strText.c_str(), m_tRenderRect, m_tFontColor, m_dwFlags);
}

void CFontUIOrtho::OnCollision(CGameObject* pObject)
{
}

CFontUIOrtho* CFontUIOrtho::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CFontUIOrtho* pFontUI = new CFontUIOrtho(pGraphicDev, pMessageChannel);

	if (FAILED(pFontUI->Ready_GameObject()))
	{
		Safe_Release(pFontUI);
		MSG_BOX("pFontUI Create Failed");
		return nullptr;
	}

	return pFontUI;
}

void CFontUIOrtho::Ready_Event()
{
}

void CFontUIOrtho::Free()
{
	CGameObject::Free();
}
