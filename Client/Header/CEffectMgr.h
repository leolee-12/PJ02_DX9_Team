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
		EK_HIT,
		EK_SLASH,
		EK_DUST_LAND,
		EK_DUST_RUN,
		EK_ENEMYSPAWN,
		EK_MONO_BLACK,
		EK_MONO_WHITE,
	};

private:
	explicit CEffectMgr();
	virtual ~CEffectMgr();

public:
	HRESULT			Ready_EffectMgr(LPDIRECT3DDEVICE9 pGraphicDev);
	_int			Update_Effect(const _float& fTimeDelta);
	void			LateUpdate_Effect(const _float& fTimeDelta);


	void			Create_Effect(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_KEY eEffectKey, const _vec3& vPos, const _vec3& vOffset = _vec3(0.f, 0.f, 0.f));
	void			Create_Attached_Effect(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_KEY eEffectKey, CGameObject* pOwner, const _vec3& vOffset = _vec3(0.f, 0.f, 0.f));
	void			Clear_Effect();
	

private:
	list<CEffect*> m_EffectList;

private:
	virtual void Free();
};
