#include "pch.h"
#include "CResourceWorkBar.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CResourceWorkFront.h"
#include "CMonsterHpBarCover.h"
#include "CDInputMgr.h"
#include "CFontMgr.h"


CResourceWorkBar::CResourceWorkBar(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CResourceWorkBar::~CResourceWorkBar()
{
}

HRESULT CResourceWorkBar::Ready_GameObject()
{
	CGameObject* pGameObject = nullptr;

	Calcute_WorldPos();

	pGameObject = m_pResourceWorkBarFront = CResourceWorkFront::Create(m_pGraphicDev, m_vCalcutePos);
	if (nullptr == pGameObject)	// 필요없는 로직 제거하도록 새로운 클래스로 사용
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);

	pGameObject = m_pResourceWorkBarCover = CMonsterHpBarCover::Create(m_pGraphicDev, m_vCalcutePos);
								// HpBarCover와 똑같으므로 같은 클래스 재활용
	if (nullptr == pGameObject)
		return E_FAIL;

	m_vecHpBarUI.push_back(pGameObject);


	m_fCurWork = m_fMaxWork;
	m_fPrevWork = m_fMaxWork;
	m_fLefpPrevHp = m_fMaxWork;
	m_fLerpTime = 0.0f;
	m_pResourceWorkBarFront->InitHp(m_fMaxWork, m_fCurWork);

	//Active();
	return S_OK;
}

_int CResourceWorkBar::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return NOEVENT; }
	
	Calcute_WorldPos();

	m_pResourceWorkBarCover->Set_Pos(m_vCoverPos);
	m_pResourceWorkBarFront->Set_Pos(m_vCalcutePos);

	if (m_fCurWork != m_fPrevWork)
	{
		m_fRadio = m_fCurWork / m_fMaxWork;
		m_pResourceWorkBarFront->Set_Radio(m_fRadio);
		m_pResourceWorkBarFront->Set_Hp(m_fCurWork);
		m_fLerpTime = 1.0f;
		m_fLefpPrevHp = m_fPrevWork;
		m_fPrevWork = m_fCurWork;
	}

	for (CGameObject*& HpBarUI : m_vecHpBarUI)
	{
		HpBarUI->Update_GameObject(fTimeDelta);
	}

	return NOEVENT;
}

void CResourceWorkBar::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return; }
	for (CGameObject*& HpBarUI : m_vecHpBarUI)
	{
		HpBarUI->LateUpdate_GameObject(fTimeDelta);
	}
}

void CResourceWorkBar::Render_GameObject()
{
}

void CResourceWorkBar::OnCollision(CGameObject* pObject)
{

}

void CResourceWorkBar::Calcute_WorldPos()
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

void		CResourceWorkBar::Active()
{
	m_bActive = true;
}

void		CResourceWorkBar::UnActive()
{
	m_bActive = false;
}

CResourceWorkBar* CResourceWorkBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _MaxHp, const _vec3& vTargetPos)
{
	CResourceWorkBar* pResourceWorkBar = new CResourceWorkBar(pGraphicDev);
	pResourceWorkBar->m_fMaxWork = _MaxHp;
	pResourceWorkBar->m_vTargetPos = vTargetPos;

	if (FAILED(pResourceWorkBar->Ready_GameObject()))
	{
		Safe_Release(pResourceWorkBar);
		MSG_BOX("pResourceWorkBar Create Failed");
		return nullptr;
	}

	return pResourceWorkBar;
}


void CResourceWorkBar::ApplyDamage(_float _fDamage)
{
	m_fCurWork += _fDamage;
	if (m_fCurWork > m_fMaxWork)
		m_fCurWork = m_fMaxWork;
	if (m_fCurWork < 0)
		m_fCurWork = 0;
}

void CResourceWorkBar::Free()
{
	for (CGameObject*& HpBarUI : m_vecHpBarUI)
	{
		Safe_Release(HpBarUI);
	}
	m_vecHpBarUI.clear();

	CUi::Free();
}
