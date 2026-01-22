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
	if (m_bReady) return S_OK;

	m_pGraphicDev = pGraphicDev;

	CSpriteEffect* pSpriteEffect = nullptr;

	auto spritePair = m_mapProtoEffect.try_emplace(EK_HIT, nullptr);	// pair<iter, bool>

	if (spritePair.second)	// 삽입 실패 시 삭제 (X) : 불필요한 생성 -> 삽입 성공 시에 생성 (O)
	{
		CSpriteEffect* pSpriteEffect = CSpriteEffect::Create(pGraphicDev, L"Proto_HitEffectTexture");
		pSpriteEffect->Set_SpriteData(CSpriteEffect::SPRITE_DATA(8, 4, 8, 24.f));  // 8x4 그리드, 8프레임, 24fps
		pSpriteEffect->Set_Scale(_vec3(25.f, 25.f, 25.f));
		pSpriteEffect->Set_Billboard(false);
		spritePair.first->second = pSpriteEffect;
	}

	spritePair = m_mapProtoEffect.try_emplace(EK_PLAYERHIT, nullptr);	// pair<iter, bool>

	if (spritePair.second)
	{
		CSpriteEffect* pSpriteEffect = CSpriteEffect::Create(pGraphicDev, L"Proto_HitEffectTexture");
		pSpriteEffect->Set_SpriteData(CSpriteEffect::SPRITE_DATA(8, 4, 8, 24.f));  // 8x4 그리드, 8프레임, 24fps
		pSpriteEffect->Set_Scale(_vec3(10.f, 10.f, 10.f));
		spritePair.first->second = pSpriteEffect;
	}

	spritePair = m_mapProtoEffect.try_emplace(EK_PICKUP, nullptr);	// pair<iter, bool>

	if (spritePair.second)
	{
		CSpriteEffect* pSpriteEffect = CSpriteEffect::Create(pGraphicDev, L"Proto_PickUpEffectTexture");
		pSpriteEffect->Set_SpriteData(CSpriteEffect::SPRITE_DATA(8, 1, 6, 24.f));  // 8x1 그리드, 6프레임, 24fps
		pSpriteEffect->Set_Scale(_vec3(3.f, 3.f, 3.f));
		spritePair.first->second = pSpriteEffect;
	}

	spritePair = m_mapProtoEffect.try_emplace(EK_ENEMYSPAWN, nullptr);	// pair<iter, bool>

	if (spritePair.second)
	{
		CSpriteEffect* pSpriteEffect = CSpriteEffect::Create(pGraphicDev, L"Proto_SpawnEffectTexture");
		pSpriteEffect->Set_SpriteData(CSpriteEffect::SPRITE_DATA(16, 4, 36, 24.f));  // 16x4 그리드, 36프레임, 12fps
		pSpriteEffect->Set_Scale(_vec3(7.f, 28.f, 7.f));
		pSpriteEffect->Set_Billboard(false);
		spritePair.first->second = pSpriteEffect;
	}

	auto particlePair = m_mapProtoEffect.try_emplace(EK_PARTICLE_GREEN, nullptr);	// pair<iter, bool>

	if (particlePair.second)
	{
		CParticleEffect* pParticleEffect = CParticleEffect::Create(pGraphicDev, L"Proto_MonsterParticleTexture");
		pParticleEffect->Set_TextureRange(0, 31);
		pParticleEffect->Set_EmitRange(_vec3(-1.f, 0.f, -1.f), _vec3(1.f, 1.f, 1.f));
		pParticleEffect->Set_SpeedRange(_vec3(-3.f, 5.f, -3.f), _vec3(3.f, 10.f, 3.f));
		pParticleEffect->Set_LifeTime(1.5f);
		pParticleEffect->Set_EmitRate(30.f);
		pParticleEffect->Set_BaseColor(D3DXCOLOR(0.1f, 1.f, 0.1f, 1.f));
		particlePair.first->second = pParticleEffect;
	}

	particlePair = m_mapProtoEffect.try_emplace(EK_PARTICLE_RED, nullptr);	// pair<iter, bool>

	if (particlePair.second)
	{
		CParticleEffect* pParticleEffect = CParticleEffect::Create(pGraphicDev, L"Proto_MonsterParticleTexture");
		pParticleEffect->Set_TextureRange(0, 31);
		pParticleEffect->Set_EmitRange(_vec3(-1.f, 0.f, -1.f), _vec3(1.f, 1.f, 1.f));
		pParticleEffect->Set_SpeedRange(_vec3(-3.f, 5.f, -3.f), _vec3(3.f, 10.f, 3.f));
		pParticleEffect->Set_LifeTime(1.5f);
		pParticleEffect->Set_EmitRate(30.f);
		pParticleEffect->Set_BaseColor(D3DXCOLOR(1.f, 0.1f, 0.1f, 1.f));
		particlePair.first->second = pParticleEffect;
	}

	particlePair = m_mapProtoEffect.try_emplace(EK_PARTICLE_BLUE, nullptr);	// pair<iter, bool>

	if (particlePair.second)
	{
		CParticleEffect* pParticleEffect = CParticleEffect::Create(pGraphicDev, L"Proto_MonsterParticleTexture");
		pParticleEffect->Set_TextureRange(0, 31);
		pParticleEffect->Set_EmitRange(_vec3(-1.f, 0.f, -1.f), _vec3(1.f, 1.f, 1.f));
		pParticleEffect->Set_SpeedRange(_vec3(-3.f, 5.f, -3.f), _vec3(3.f, 10.f, 3.f));
		pParticleEffect->Set_LifeTime(1.5f);
		pParticleEffect->Set_EmitRate(30.f);
		pParticleEffect->Set_BaseColor(D3DXCOLOR(0.1f, 0.1f, 1.f, 1.f));
		particlePair.first->second = pParticleEffect;
	}

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

	auto trailPair = m_mapProtoEffect.try_emplace(EK_TRAIL_GREEN, nullptr);	// pair<iter, bool>

	if (trailPair.second)
	{
		CTrailEffect* pTrailEffect = CTrailEffect::Create(pGraphicDev, L"Proto_GreenTrailTexture");
		pTrailEffect->Set_Color(D3DXCOLOR(0.0f, 1.f, 0.0f, 1.f));
		pTrailEffect->Set_HeadSize(0.5f);
		pTrailEffect->Set_TailLength(2.f);
		pTrailEffect->Set_Emissive(true);
		particlePair.first->second = pTrailEffect;
	}

	m_bReady = true;

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

CEffect* CEffectMgr::Create_Effect(	EFFECT_KEY eEffectKey, const _uint& iTexIdx, const _vec3& vPos,
									const _vec3& vOffset, CGameObject* pOwner)
{
	auto iter = m_mapProtoEffect.find(eEffectKey);

	if (iter == m_mapProtoEffect.end())
	{
		MSG_BOX("Effect Not Found");
		return nullptr;
	}

	CEffect* pEffect = dynamic_cast<CEffect*>(iter->second->Clone());

	if (!pEffect)
	{
		MSG_BOX("Effect Clone Failed");
		return nullptr;
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

	return pEffect;
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
