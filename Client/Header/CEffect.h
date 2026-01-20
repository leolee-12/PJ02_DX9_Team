#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CEffect : public CGameObject
{
public:
	enum EFFECT_TYPE{ EF_HIT, EF_PICKUP, EF_PARTICLE, EF_FOG, EF_END };
private:
	explicit	CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CEffect(const CEffect& rhs);
	virtual		~CEffect();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();


private:
	HRESULT			Add_Component();
	void			Ready_Variable();
	void			Set_Texture();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

	// 스프라이트 관련
	_float		m_fFrame;
	_float		m_fFrameEnd;
	_float		m_fFrameSpeed;
	EFFECT_TYPE m_eEffectType;
	_uint		m_iTexIdx;
	_matrix		m_matTex;

	_float		m_fVar = 0.2f;

public:
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _uint iTexIdx);

private:
	virtual void Free();

};

