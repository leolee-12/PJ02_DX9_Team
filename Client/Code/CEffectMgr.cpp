#include "pch.h"
#include "CEffectMgr.h"
#include "CEffect.h"
#include "CTransform.h"

IMPLEMENT_SINGLETON(CEffectMgr);

CEffectMgr::CEffectMgr()
{
}

CEffectMgr::~CEffectMgr()
{
	Free();
}

HRESULT CEffectMgr::Ready_EffectMgr(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return S_OK;
}

_int CEffectMgr::Update_Effect(const _float& fTimeDelta)
{
	for (auto iter = m_EffectList.begin(); iter != m_EffectList.end(); ++iter)
	{
		_int iExit = (*iter)->Update_GameObject(fTimeDelta);

		if (iExit == DEAD)
		{
			Safe_Release((*iter));
			iter = m_EffectList.erase(iter);
		}
		else ++iter;
	}

	return NOEVENT;
}

void CEffectMgr::LateUpdate_Effect(const _float& fTimeDelta)
{
	for (auto& pEffect : m_EffectList)
	{
		pEffect->LateUpdate_GameObject(fTimeDelta);
	}
}

void CEffectMgr::Create_Effect(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_KEY eEffectKey, const _vec3& vPos, const _vec3& vOffset)
{
	CEffect* pEffect = CEffect::Create(pGraphicDev, vPos, _uint(eEffectKey));

	if (pEffect)	m_EffectList.push_back(pEffect);
	else			MSG_BOX("Create_Effect Failed");
}

void CEffectMgr::Create_Attached_Effect(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_KEY eEffectKey, CGameObject* pOwner, const _vec3& vOffset)
{
	_vec3 vPos;
	static_cast<CTransform*>(pOwner->Get_Component(ID_DYNAMIC, L"Com_Transform"))->Get_Info(INFO_POS, &vPos);

	CEffect* pEffect = CEffect::Create(pGraphicDev, vPos, _uint(eEffectKey));

	if (pEffect)	m_EffectList.push_back(pEffect);
	else			MSG_BOX("Create_Attached_Effect Failed");
}

void CEffectMgr::Clear_Effect()
{
	for (auto& pEffect : m_EffectList)
	{
		Safe_Release(pEffect);
	}

	m_EffectList.clear();
}

void CEffectMgr::Free()
{
	Clear_Effect();
}
