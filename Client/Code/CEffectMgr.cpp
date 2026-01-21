#include "pch.h"
#include "CEffectMgr.h"
#include "CEffect.h"
#include "CTransform.h"
#include "CSpriteEffect.h"
#include "CParticleEffect.h"
#include "CTrailEffect.h"
#include "CScreenEffect.h"

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
	m_pGraphicDev = pGraphicDev;

	CSpriteEffect* pSpriteEffect = nullptr;
	pSpriteEffect = CSpriteEffect::Create(pGraphicDev, L"Proto_HitEffectTexture");
	pSpriteEffect->Set_SpriteData({ 8, 4, 8, 24.f });  // 8x4 그리드, 8프레임, 24fps
	pSpriteEffect->Set_Scale(25.f);
	m_mapProtoEffect.emplace(EK_HIT, pSpriteEffect);

	pSpriteEffect = nullptr;
	pSpriteEffect = CSpriteEffect::Create(pGraphicDev, L"Proto_HitEffectTexture");
	pSpriteEffect->Set_SpriteData({ 8, 4, 8, 24.f });  // 8x4 그리드, 8프레임, 24fps
	pSpriteEffect->Set_Scale(10.f);
	m_mapProtoEffect.emplace(EK_PLAYERHIT, pSpriteEffect);

	pSpriteEffect = CSpriteEffect::Create(pGraphicDev, L"Proto_PickUpEffectTexture");
	pSpriteEffect->Set_SpriteData({ 8, 1, 6, 24.f });  // 8x1 그리드, 8프레임, 24fps
	pSpriteEffect->Set_Scale(3.f);
	m_mapProtoEffect.emplace(EK_PICKUP, pSpriteEffect);

	//CParticleEffect* pDustLand = CParticleEffect::Create(pGraphicDev);
	//pDustLand->Set_TextureKey(L"Proto_DustTexture");
	//pDustLand->Set_EmitRange(_vec3(-0.5f, 0.f, -0.5f), _vec3(0.5f, 0.f, 0.5f));
	//pDustLand->Set_SpeedRange(_vec3(-2.f, 1.f, -2.f), _vec3(2.f, 3.f, 2.f));
	//pDustLand->Set_LifeTime(0.5f);
	//pDustLand->Set_EmitRate(30.f);
	//m_mapProtoEffect.emplace(EK_DUST_LAND, pDustLand);
	//
	//
	//CScreenEffect* pConc = CScreenEffect::Create(pGraphicDev);
	//pConc->Set_TextureKey(L"Proto_ConcentrationTexture");
	//pConc->Set_SEFType(CScreenEffect::SEF_DARK);
	//pConc->Set_FadeTime(0.1f, 0.2f);  // fadeIn, fadeOut
	//m_mapProtoEffect.emplace(EK_MONO_BLACK, pConc);

	return S_OK;
}

_int CEffectMgr::Update_Effect(const _float& fTimeDelta)
{
	for (auto iter = m_EffectList.begin(); iter != m_EffectList.end();)
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

void CEffectMgr::Create_Effect(	EFFECT_KEY eEffectKey, const _uint& iTexIdx, const _vec3& vPos,
								const _vec3& vOffset, CGameObject* pOwner)
{
	auto iter = m_mapProtoEffect.find(eEffectKey);

	if (iter == m_mapProtoEffect.end())
	{
		MSG_BOX("Effect Not Found");
		return;
	}

	CEffect* pEffect = dynamic_cast<CEffect*>(iter->second->Clone());

	if (!pEffect)
	{
		MSG_BOX("Effect Clone Failed");
		return;
	}

	pEffect->Set_TextureIndex(iTexIdx);

	CTransform* pTransformCom = static_cast<CTransform*>(pEffect->Get_Component(ID_DYNAMIC, L"Com_Transform"));

	_vec3 vFinalPos = vPos;
	vFinalPos.x += Get_Rand_Float(-vOffset.x, vOffset.x);
	vFinalPos.y += Get_Rand_Float(-vOffset.y, vOffset.y);
	vFinalPos.z += Get_Rand_Float(-vOffset.z, vOffset.z);

	pTransformCom->Set_Pos(vFinalPos.x, vFinalPos.y, vFinalPos.z);

	if (pOwner) pEffect->Set_Owner(pOwner);

	pEffect->Play();

	m_EffectList.push_back(pEffect);
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

	for (auto& pair : m_mapProtoEffect)
	{
		Safe_Release(pair.second);
	}

	m_mapProtoEffect.clear();
}
