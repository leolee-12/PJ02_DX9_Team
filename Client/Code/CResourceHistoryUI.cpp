#include "pch.h"
#include "CResourceHistoryUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CResourceHistoryBack.h"
#include "CResourceHistoryDeco.h"
#include "CResourceHistoryItem.h"
#include "CFontUIOrtho.h"


#include "CDInputMgr.h"
#include "CFontMgr.h"

CResourceHistoryUI::CResourceHistoryUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_bRender(true)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CResourceHistoryUI::~CResourceHistoryUI()
{
}

HRESULT CResourceHistoryUI::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	pGameObject = m_pResourceHistoryBack = CResourceHistoryBack::Create(m_pGraphicDev, _vec3(-WINCX / 2 + m_vPos.x, -m_vPos.y + WINCY / 2, m_vPos.z), 0.35f);
	if (nullptr == pGameObject)
		return E_FAIL;

	pGameObject = m_pResourceHistoryDeco = CResourceHistoryDeco::Create(m_pGraphicDev, _vec3(-WINCX / 2 + m_vPos.x, -m_vPos.y + WINCY / 2, m_vPos.z), 0.35f);
	if (nullptr == pGameObject)
		return E_FAIL;


	pGameObject = m_pResourceHistoryItem = CResourceHistoryItem::Create(m_pGraphicDev, _vec3(-WINCX / 2 + m_vPos.x - 60.0f, -m_vPos.y + WINCY / 2, m_vPos.z), 0.35f, m_iIndex);
	if (nullptr == pGameObject)
		return E_FAIL;

	pGameObject = m_pTotalCountFont = CFontUIOrtho::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pTotalCountFont->Set_Flags(DT_LEFT | DT_VCENTER);
	m_pTotalCountFont->Set_FontColor(D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.5f));
	m_pTotalCountFont->Set_Pos(_vec2(0.0f, 0.0f));
	m_pTotalCountFont->Set_Scale(_vec2(59.f * 2.f, 123.f * 0.5f));
	m_pTotalCountFont->Set_Font(L"Font_Default24");
	m_pTotalCountFont->Active();

	pGameObject = m_pNameFont = CFontUIOrtho::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pNameFont->Set_Flags(DT_LEFT | DT_VCENTER);
	m_pNameFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pNameFont->Set_Pos(_vec2(0.0f, 0.0f));
	m_pNameFont->Set_Scale(_vec2(59.f * 2.f, 123.f * 0.5f));
	m_pNameFont->Set_Font(L"Font_Default");
	m_pNameFont->Active();
	m_pNameFont->Set_Text(m_szName);

	pGameObject = m_pDeltaAmountFont = CFontUIOrtho::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pDeltaAmountFont->Set_Flags(DT_LEFT | DT_VCENTER);
	m_pDeltaAmountFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pDeltaAmountFont->Set_Pos(_vec2(0.0f, 0.0f));
	m_pDeltaAmountFont->Set_Scale(_vec2(59.f * 2.f, 123.f * 0.5f));
	m_pDeltaAmountFont->Set_Font(L"Font_Default30_Heavy");
	m_pDeltaAmountFont->Active();


	m_iTotalCount = 0;
	m_fLerpTime = 1.1f;
	m_fDisplayTimeMax = 5.0f;
	Reset_DisPlayTime();
	m_bRender = true;
	m_iTotalCount = 0;
	m_bClosing = false;
	m_EndLerpTime = 1.0f;
	m_iDeltaAmount = 0;
	m_iOrder = -1;
	return S_OK;
}

_int CResourceHistoryUI::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }

	m_pResourceHistoryBack->Update_GameObject(fTimeDelta);
	m_pResourceHistoryDeco->Update_GameObject(fTimeDelta);
	m_pResourceHistoryItem->Update_GameObject(fTimeDelta);
	m_pTotalCountFont->Update_GameObject(fTimeDelta);
	m_pNameFont->Update_GameObject(fTimeDelta);
	m_pDeltaAmountFont->Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return NOEVENT;
}

void CResourceHistoryUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }

	if (m_bClosing)
	{
		if (m_EndLerpTime <= 1.0f)
		{
			D3DXVec3Lerp(&m_vPos, &m_vStartLerpPos, &m_vEndLerpPos, m_EndLerpTime);
			m_EndLerpTime += fTimeDelta;
		}
	}
	else
	{
		if (m_fDisplayTime >= 0.0f)
		{
			m_fDisplayTime -= fTimeDelta;
		}
		else if(m_iOrder == 0 && m_fLerpTime >= 1.0f)
		{
			IMessageChannel::EVENT tEvent;
			tEvent.strType = L"ResourceHistory.Close";
			tEvent.hmapData[L"ItemIndex"] = m_iIndex;

			m_pMessageChannel->Publish(tEvent);
			_vec3	EndPos = _vec3{ m_vPos.x - 300.0f, m_vPos.y, m_vPos.z };

			Moveto(m_vPos, EndPos);
			m_EndLerpTime = 0.0f;
			m_bClosing = true;
		}

		if (m_fLerpTime <= 1.0f)
		{
			D3DXVec3Lerp(&m_vPos, &m_vStartLerpPos, &m_vEndLerpPos, m_fLerpTime);
			m_fLerpTime += fTimeDelta;

		}
		else
		{
			m_fLerpTime = 1.0f;
		}
	}


	m_pResourceHistoryBack->Set_Pos(m_vPos);
	m_pResourceHistoryDeco->Set_Pos(m_vPos);
	m_pResourceHistoryItem->Set_Pos(m_vPos);
	m_pResourceHistoryBack->LateUpdate_GameObject(fTimeDelta);
	m_pResourceHistoryDeco->LateUpdate_GameObject(fTimeDelta);
	m_pResourceHistoryItem->LateUpdate_GameObject(fTimeDelta);
	m_pTotalCountFont->LateUpdate_GameObject(fTimeDelta);
	m_pNameFont->LateUpdate_GameObject(fTimeDelta);
	m_pDeltaAmountFont->LateUpdate_GameObject(fTimeDelta);

	m_pTotalCountFont->Set_Pos(_vec2(m_vPos.x - WINCX / 2 + 100.0f, -m_vPos.y + WINCY / 2));
	m_pNameFont->Set_Pos(_vec2(m_vPos.x - WINCX / 2 + 60.0f, -m_vPos.y + WINCY / 2));
	m_pDeltaAmountFont->Set_Pos(_vec2(m_vPos.x - WINCX / 2 + 20.0f, -m_vPos.y + WINCY / 2));
	wchar_t szTotalCount[8];
	swprintf_s(szTotalCount, L"%d", m_iTotalCount);
	m_pTotalCountFont->Set_Text(szTotalCount);

	if (m_iDeltaAmount > 0)
	{
		m_pDeltaAmountFont->Set_Text((L"+" + to_wstring(m_iDeltaAmount)).c_str());
	}
	else
	{
		m_pDeltaAmountFont->Set_Text((L"-" + to_wstring(m_iDeltaAmount)).c_str());
	}
}

void CResourceHistoryUI::Render_GameObject()
{
	if (!m_bRender) { return; }

	m_pDeltaAmountFont->Render_GameObject();
	m_pNameFont->Render_GameObject();
	m_pTotalCountFont->Render_GameObject();
}

void CResourceHistoryUI::OnCollision(CGameObject* pObject)
{

}

CResourceHistoryUI* CResourceHistoryUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, _vec3 _vPos, const wstring& _szName, _int _iIndex)
{
	CResourceHistoryUI* pCResourceHistoryUI = new CResourceHistoryUI(pGraphicDev);
	pCResourceHistoryUI->m_szName = _szName;
	pCResourceHistoryUI->m_vPos = _vPos;
	pCResourceHistoryUI->m_iIndex = _iIndex;

	if (FAILED(pCResourceHistoryUI->Ready_GameObject()))
	{
		Safe_Release(pCResourceHistoryUI);
		MSG_BOX("pCResourceHistoryUI Create Failed");
		return nullptr;
	}

	return pCResourceHistoryUI;
}

void CResourceHistoryUI::Set_MessageChannel(IMessageChannel* pMessageChannel)
{
	if (pMessageChannel == nullptr) { return; }
	if (m_pMessageChannel) {
		CGameObject::Unsubscribe_Handles();
		Safe_Release(m_pMessageChannel);
	}
	m_pMessageChannel = pMessageChannel;
	m_pMessageChannel->AddRef();
}


void CResourceHistoryUI::Free()
{

	Safe_Release(m_pResourceHistoryBack);
	Safe_Release(m_pResourceHistoryDeco);
	Safe_Release(m_pResourceHistoryItem);
	Safe_Release(m_pDeltaAmountFont);
	Safe_Release(m_pNameFont);
	Safe_Release(m_pTotalCountFont);
	CUi::Free();
}
