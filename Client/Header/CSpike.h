#pragma once
#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

// ==========================
//	CSpike : 보스 몬스터 공격용 클래스
// ==========================

class CSpike : public CGameObject
{
private:
	explicit	CSpike(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CSpike(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CSpike(const CSpike& rhs);
	virtual		~CSpike();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) {};

	void				Set_SpikeScale(const _vec3& vScale);
	void				Set_SpikePos(const _vec3& vPos);
	void				Set_SpikeDir(const _vec3& vDir) { D3DXVec3Normalize(&m_vDir, &vDir); }
	void				Set_Depth(const _float& fDepth) { m_fDepth = fDepth; }
	const _vec3&		Get_SpikePos() { return m_vPos; }
	const _vec3&		Get_SpikeDir() { return m_vDir; }

private:
	HRESULT				Add_Component();

	void				Ready_Variable();
	void				Move_Frame(const _float& fTimeDelta);
	void				Set_Texture();
	void				Recur_Spike();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCollider*	m_pColliderCom;

	_vec3				m_vPos;
	_vec3				m_vDir;

	// 스프라이트 관련
	_float				m_fFrame;
	_float				m_fFrameEnd;
	_float				m_fFrameSpeed;
	_matrix				m_matTex;
	_uint				m_iTexIdx;

	// 패턴 관련
	_vec3			m_vSpeed;
	_float			m_fGroundY;
	_uint			m_iRecurCount;

public:
	static CSpike* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos, _vec3 vSpeed, _uint iRecurred = 0, _uint iTexIdx = 0);

private:
	virtual void		Free();
};
