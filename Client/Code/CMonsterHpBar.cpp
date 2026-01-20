#include "pch.h"
#include "CMonsterHpBar.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CMonsterHpFront.h"
#include "CMonsterHpBarCover.h"

#include "CDInputMgr.h"
#include "CFontMgr.h"


CMonsterHpBar::CMonsterHpBar(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CMonsterHpBar::~CMonsterHpBar()
{
}

HRESULT CMonsterHpBar::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	Calcute_WorldPos();

	pGameObject = m_pMonsterHpBarFront = CMonsterHpFront::Create(m_pGraphicDev, m_vCalcutePos);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);

	pGameObject = m_pMonsterHpBarCover = CMonsterHpBarCover::Create(m_pGraphicDev, m_vCalcutePos);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);


	m_fCurHp = m_fMaxHp;
	m_fPrevHp = m_fMaxHp;
	m_fLefpPrevHp = m_fMaxHp;
	m_fLerpTime = 0.0f;
	m_pMonsterHpBarFront->InitHp(m_fMaxHp, m_fCurHp);


	//Active();
	return S_OK;
}

_int CMonsterHpBar::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return NOEVENT; }

	Calcute_WorldPos();

	m_pMonsterHpBarCover->Set_Pos(m_vCoverPos);
	m_pMonsterHpBarFront->Set_Pos(m_vCalcutePos);

	if (m_fLerpTime > 0.0f)
	{
		float LerpHp = Lerp(m_fCurHp, m_fLefpPrevHp, m_fLerpTime);
		m_pMonsterHpBarFront->Set_LerpHp(LerpHp / m_fMaxHp);
		m_fLerpTime -= fTimeDelta;
	}
	else if (m_fLerpTime <= 0.0f)
	{
		m_fLerpTime = 0.f;
		float LerpHp = Lerp(m_fCurHp, m_fLefpPrevHp, m_fLerpTime);
		m_fRadio = m_fCurHp / m_fMaxHp;
		m_pMonsterHpBarFront->Set_LerpHp(LerpHp / m_fMaxHp);
	}

	if (m_fCurHp != m_fPrevHp)
	{
		m_fRadio = m_fCurHp / m_fMaxHp;
		m_pMonsterHpBarFront->Set_Radio(m_fRadio);
		m_pMonsterHpBarFront->Set_Hp(m_fCurHp);
		m_fLerpTime = 1.0f;
		m_fLefpPrevHp = m_fPrevHp;
		m_fPrevHp = m_fCurHp;
	}

	for (CGameObject*& HpBarUI : m_vecHpBarUI)
	{
		HpBarUI->Update_GameObject(fTimeDelta);
	}

	return NOEVENT;
}

void CMonsterHpBar::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return; }
	for (CGameObject*& HpBarUI : m_vecHpBarUI)
	{
		HpBarUI->LateUpdate_GameObject(fTimeDelta);
	}
}

void CMonsterHpBar::Render_GameObject()
{
}

void CMonsterHpBar::OnCollision(CGameObject* pObject)
{

}

void CMonsterHpBar::Calcute_WorldPos()
{
	_matrix matView, matProj;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	_vec3 vViewPos, vndcPos, vScreenPos;
	D3DXVec3TransformCoord(&vViewPos, &m_vTargetPos, &matView);
	D3DXVec3TransformCoord(&vndcPos, &vViewPos, &matProj);

	vScreenPos.x = (vndcPos.x * 0.5f + 0.5f) * _float(WINCX);
	vScreenPos.y = (-vndcPos.y * 0.5f + 0.5f) * _float(WINCY);

	m_vCalcutePos = _vec3(vScreenPos.x - _float(WINCX / 2), -vScreenPos.y + _float(WINCY / 2), 0.5f);
	m_vCoverPos = m_vCalcutePos;
	m_vCoverPos.z += 0.01f;
}

void		CMonsterHpBar::Active()
{
	m_bActive = true;
}

void		CMonsterHpBar::UnActive()
{
	m_bActive = false;
}

CMonsterHpBar* CMonsterHpBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _MaxHp, const _vec3& vTargetPos)
{
	CMonsterHpBar* pCMonsterHpBar = new CMonsterHpBar(pGraphicDev);
	pCMonsterHpBar->m_fMaxHp = _MaxHp;
	pCMonsterHpBar->m_vTargetPos = vTargetPos;

	if (FAILED(pCMonsterHpBar->Ready_GameObject()))
	{
		Safe_Release(pCMonsterHpBar);
		MSG_BOX("pCMonsterHpBar Create Failed");
		return nullptr;
	}

	return pCMonsterHpBar;
}


void CMonsterHpBar::ApplyDamage(_float _fDamage)
{
	m_fCurHp += _fDamage;
	if (m_fCurHp > m_fMaxHp)
		m_fCurHp = m_fMaxHp;
	if (m_fCurHp < 0)
		m_fCurHp = 0;
}

void CMonsterHpBar::Free()
{
	for (CGameObject*& HpBarUI : m_vecHpBarUI)
	{
		Safe_Release(HpBarUI);
	}
	m_vecHpBarUI.clear();

	CUi::Free();
}
