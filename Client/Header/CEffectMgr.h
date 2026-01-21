#pragma once
#include "CBase.h"
#include "Engine_Define.h"

namespace Engine
{
	class CGameObject;
}

class CEffect;

class CEffectMgr : public CBase
{
	DECLARE_SINGLETON(CEffectMgr)

public:
	enum EFFECT_KEY
	{
		// Sprite
		EK_HIT
		, EK_PLAYERHIT
		, EK_PICKUP
		, EK_ENEMYSPAWN

		// Particle
		, EK_PARTICLE_GREEN
		, EK_PARTICLE_RED
		, EK_PARTICLE_PURPLE
		//,EK_DUST_LAND
		//,EK_DUST_RUN

		// Screen
		//,EK_MONO_BLACK
		//,EK_MONO_WHITE
	};

private:
	explicit CEffectMgr();
	virtual ~CEffectMgr();

public:
	HRESULT			Ready_EffectMgr(LPDIRECT3DDEVICE9 pGraphicDev);
	_int			Update_Effect(const _float& fTimeDelta);
	void			LateUpdate_Effect(const _float& fTimeDelta);

	void			Create_Effect(	EFFECT_KEY eEffectKey, const _uint& iTexIdx, const _vec3& vPos,
									const _vec3& vOffset = _vec3(0.f, 0.f, 0.f), CGameObject * pOwner = nullptr);
	void			Clear_Effect();
	

private:
	LPDIRECT3DDEVICE9					m_pGraphicDev;
	list<CEffect*>						m_EffectList;
	unordered_map<EFFECT_KEY, CEffect*>	m_mapProtoEffect;
	_bool								m_bReady = false;

private:
	virtual void Free();
};
