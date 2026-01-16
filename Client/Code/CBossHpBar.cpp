#include "pch.h"
#include "CBossHpBar.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CBossHpBarFront.h"
#include "CBossHpBarMiddle.h"
#include "CBossHpBarBackground.h"

#include "CFontMgr.h"


CBossHpBar::CBossHpBar(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_bRender(true)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CBossHpBar::~CBossHpBar()
{
}

HRESULT CBossHpBar::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	pGameObject = CBossHpBarFront::Create(m_pGraphicDev,{0,-WINCY / 2 + 50,0.001f},3.0f);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);


	pGameObject = CBossHpBarMiddle::Create(m_pGraphicDev, { 0,-WINCY / 2 + 50,0.01f }, 3.0f);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);

	pGameObject = CBossHpBarBackground::Create(m_pGraphicDev, { 0,-WINCY / 2 + 50,0.1f }, 3.0f);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);

	return S_OK;
}

_int CBossHpBar::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	for (CGameObject* CookingUI : m_vecHpBarUI)
	{
		CookingUI->Update_GameObject(fTimeDelta);
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);



	return NOEVENT;
}

void CBossHpBar::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	for (CGameObject* CookingUI : m_vecHpBarUI)
	{
		CookingUI->LateUpdate_GameObject(fTimeDelta);
	}
}

void CBossHpBar::Render_GameObject()
{
	if (!m_bRender) { return; }

}

void CBossHpBar::OnCollision(CGameObject* pObject)
{

}

CBossHpBar* CBossHpBar::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossHpBar* pCookingMiniGame = new CBossHpBar(pGraphicDev);

	if (FAILED(pCookingMiniGame->Ready_GameObject()))
	{
		Safe_Release(pCookingMiniGame);
		MSG_BOX("pBossHpBar Create Failed");
		return nullptr;
	}

	return pCookingMiniGame;
}


void CBossHpBar::Free()
{
	for (CGameObject* CookingUI : m_vecHpBarUI)
	{
		Safe_Release(CookingUI);
	}
	m_vecHpBarUI.clear();

	CUi::Free();
}
