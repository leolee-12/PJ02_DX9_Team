#include "pch.h"
#include "CBossHpBar.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CBossHpBarFront.h"
#include "CBossHpBarMiddle.h"
#include "CBossHpBarBackground.h"

#include "CDInputMgr.h"
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

	pGameObject = m_pBossHpBarFront = CBossHpBarFront::Create(m_pGraphicDev,{0,-WINCY / 2 + 50,0.001f},3.0f);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);


	pGameObject = m_pBossHpBarMiddle = CBossHpBarMiddle::Create(m_pGraphicDev, { 0,-WINCY / 2 + 50,0.01f }, 3.0f);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);

	pGameObject = CBossHpBarBackground::Create(m_pGraphicDev, { 0,-WINCY / 2 + 50,0.1f }, 3.0f);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);

	m_fCurrtHp = 100.0f;
	m_fMaxHp = 100.0f;
	m_fPrevHp = 100.0f;
	return S_OK;
}

_int CBossHpBar::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	if (CDInputMgr::GetInstance()->Key_Down(DIK_X))
	{
		ApplyDamage(-5);
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_C))
	{
		ApplyDamage(5);
	}





	if (m_fCurrtHp <= 0)
	{
		Set_Render(false);
	}

	if (m_fCurrtHp != m_fPrevHp)
	{
		m_fRadio = m_fCurrtHp / m_fMaxHp;
		m_pBossHpBarFront->Set_Radio(m_fRadio);
		m_pBossHpBarMiddle->Set_Radio(m_fRadio);
		m_fPrevHp = m_fCurrtHp;
	}

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
	CBossHpBar* pCBossHpBar = new CBossHpBar(pGraphicDev);

	if (FAILED(pCBossHpBar->Ready_GameObject()))
	{
		Safe_Release(pCBossHpBar);
		MSG_BOX("pBossHpBar Create Failed");
		return nullptr;
	}

	return pCBossHpBar;
}


void CBossHpBar::ApplyDamage(_float _fDamage)
{
	m_fCurrtHp += _fDamage;
	if(m_fCurrtHp > m_fMaxHp)
		m_fCurrtHp = m_fMaxHp;
	if (m_fCurrtHp < 0)
		m_fCurrtHp = 0;
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
