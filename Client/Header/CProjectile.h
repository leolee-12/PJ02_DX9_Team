#pragma once
#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

//class CTrailEffect;
class CIndicator;

class CProjectile : public CGameObject
{
private:
	explicit	CProjectile(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CProjectile(const CProjectile& rhs);
	virtual		~CProjectile();

public:
	void				Set_Pos(const _vec3& vPos);
	void				Set_vecSpeed(const _vec3& vSpeed) { m_vSpeed = vSpeed; }
	void				Set_UseGravity(const _bool& bUseGravity) { m_bUseGravity = bUseGravity; }

	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject);

private:
	HRESULT				Add_Component();

	void				Ready_Variable();
	void				Ready_Event();

	void				Check_Frame();
	void				Move_Frame(const _float& fTimeDelta);
	void				Set_Texture();
	void				Set_Material();
	void				Reset_Material();
private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCollider*	m_pColliderCom;

	_vec3				m_vPos;

	_float				m_fFrame;
	_float				m_fFrameEnd;
	_float				m_fFrameSpeed;
	_matrix				m_matTex;

	// 스테이터스 관련
	_int				m_iAttack;
	_float				m_fGroundY;
	_bool				m_bActive;
	_float				m_fAcmlTime;
	_float				m_fLifeTime;
	_vec3				m_vSpeed;
	_bool				m_bUseGravity;
	_float				m_fGravity;

	//CTrailEffect*		m_pTrailEffect;
	CIndicator*			m_pIndicator;
	D3DXCOLOR			m_tColor;
	_bool				m_bMtrl = false;
	COLGROUP			m_eColGroup;

public:
	static CProjectile* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vSpeed, _bool bUseGravity, COLGROUP eGroup, D3DXCOLOR tColor = D3DXCOLOR(0.f, 0.f, 0.f, 1.f));

private:
	virtual void		Free();
};
