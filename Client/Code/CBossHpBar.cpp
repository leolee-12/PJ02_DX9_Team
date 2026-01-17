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

	pGameObject = m_pBossHpBarFront = CBossHpBarFront::Create(m_pGraphicDev,{0.0f,-WINCY / 2.0f + 50,0.001f},3.0f);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);


	pGameObject = m_pBossHpBarMiddle = CBossHpBarMiddle::Create(m_pGraphicDev, { 0.0f,-WINCY / 2.0f + 50.0f,0.01f }, 3.0f);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);

	pGameObject = CBossHpBarBackground::Create(m_pGraphicDev, { 0.0f,-WINCY / 2.0f + 50.0f,0.1f }, 3.0f);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);

	m_fCurHp = m_fMaxHp;
	m_fPrevHp = m_fMaxHp;
	m_fLefpPrevHp = m_fMaxHp;
	m_fLerpTime = 0.0f;
	m_pBossHpBarFront->InitHp(m_fMaxHp, m_fCurHp);
	m_pBossHpBarMiddle->InitHp(m_fMaxHp, m_fCurHp);
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

	if (m_fCurHp <= 0)
	{
		Set_Render(false);
	}

	if (m_fLerpTime > 0.0f)
	{
		float LerpHp = Lerp(m_fCurHp, m_fLefpPrevHp , m_fLerpTime);
		m_pBossHpBarMiddle->Set_Radio(LerpHp / m_fMaxHp);
		m_pBossHpBarMiddle->Set_Hp(LerpHp / m_fMaxHp);
		m_fLerpTime -= fTimeDelta;
	}
	else if (m_fLerpTime <= 0.0f)
	{

		m_fRadio = m_fCurHp / m_fMaxHp;
		m_pBossHpBarMiddle->Set_Radio(m_fRadio);
		m_pBossHpBarMiddle->Set_Hp(m_fCurHp);
	}

	if (m_fCurHp != m_fPrevHp)
	{
		m_fRadio = m_fCurHp / m_fMaxHp;
		m_pBossHpBarFront->Set_Radio(m_fRadio);
		m_pBossHpBarFront->Set_Hp(m_fCurHp);
		m_fLerpTime = 1.0f;
		m_fLefpPrevHp = m_fPrevHp;
		m_fPrevHp = m_fCurHp;
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

CBossHpBar* CBossHpBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _MaxHp)
{
	CBossHpBar* pCBossHpBar = new CBossHpBar(pGraphicDev);
	pCBossHpBar->m_fMaxHp = _MaxHp;

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
	m_fCurHp += _fDamage;
	if(m_fCurHp > m_fMaxHp)
		m_fCurHp = m_fMaxHp;
	if (m_fCurHp < 0)
		m_fCurHp = 0;
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
