#include "pch.h"
#include "CBossHpBar.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CBossHpBarFront.h"
#include "CBossHpBarMiddle.h"
#include "CBossHpBarBackground.h"

#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CFontUIOrtho.h"


CBossHpBar::CBossHpBar(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CBossHpBar::~CBossHpBar()
{
}

HRESULT CBossHpBar::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	pGameObject = m_pBossHpBarFront = CBossHpBarFront::Create(m_pGraphicDev, _vec3(0.0f, _float(- WINCY / 2) + 50, 0.11f));
	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);

	pGameObject = CBossHpBarBackground::Create(m_pGraphicDev, _vec3(0.0f, _float(- WINCY / 2) + 50.0f, 0.12f));

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);

	pGameObject = m_pFont = CFontUIOrtho::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pFont->Set_Text(m_strOwnerName);
	m_pFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	//m_pFont->Set_Font();
	m_pFont->Set_Pos(_vec2(0.0f, _float(-WINCY / 2) + 50.0f));
	m_pFont->Set_Scale(_vec2(59.f * 8.f, 7.f * 3.f));


	m_fCurHp = m_fMaxHp;
	m_fPrevHp = m_fMaxHp;
	m_fLefpPrevHp = m_fMaxHp;
	m_fLerpTime = 0.0f;
	m_pBossHpBarFront->InitHp(m_fMaxHp, m_fCurHp);
	m_fDepth = 0.05f;


	//Active();
	return S_OK;
}

_int CBossHpBar::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return NOEVENT; }

	if (m_fLerpTime > 0.0f)
	{
		float LerpHp = Lerp(m_fCurHp, m_fLefpPrevHp , m_fLerpTime);
		m_pBossHpBarFront->Set_LerpHp(LerpHp / m_fMaxHp);
		m_fLerpTime -= fTimeDelta;
	}
	else if (m_fLerpTime <= 0.0f)
	{
		m_fLerpTime = 0.f;
		float LerpHp = Lerp(m_fCurHp, m_fLefpPrevHp, m_fLerpTime);
		m_fRadio = m_fCurHp / m_fMaxHp;
		m_pBossHpBarFront->Set_LerpHp(LerpHp / m_fMaxHp);
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

	for (CGameObject*& HpBarUI : m_vecHpBarUI)
	{
		HpBarUI->Update_GameObject(fTimeDelta);
	}

	m_pFont->Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_FONT, m_pFont);

	return NOEVENT;
}

void CBossHpBar::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return; }
	for (CGameObject*& HpBarUI : m_vecHpBarUI)
	{
		HpBarUI->LateUpdate_GameObject(fTimeDelta);
	}

	m_pFont->LateUpdate_GameObject(fTimeDelta);
}

void CBossHpBar::Render_GameObject()
{
}

void CBossHpBar::OnCollision(CGameObject* pObject)
{

}

void		CBossHpBar::Active()
{
	m_bActive = true;
	m_pFont->Active();
}

void		CBossHpBar::UnActive()
{
	m_bActive = false;
	m_pFont->UnActive();
}

CBossHpBar* CBossHpBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _MaxHp, const wstring& strOwnerName)
{
	CBossHpBar* pCBossHpBar = new CBossHpBar(pGraphicDev);
	pCBossHpBar->m_fMaxHp = _MaxHp;
	pCBossHpBar->m_strOwnerName = strOwnerName;

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
	for (CGameObject*& HpBarUI : m_vecHpBarUI)
	{
		Safe_Release(HpBarUI);
	}
	m_vecHpBarUI.clear();

	Safe_Release(m_pFont);

	CUi::Free();
}
