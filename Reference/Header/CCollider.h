#pragma once

#include "CComponent.h"
#include "Engine_Define.h"


BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider();

public:
	HRESULT Ready_Collider(const AABB& tInitAABB, COLLAYER Layer = CL_NONE, _bool bTrigger = false);
	void UpdateFromTransform(class CTransform* pTransform);

	void RegisterToManager(CBase* pOwner);
	void UnregisterFromManager();

	const AABB& Get_AABB() const { return m_tAABB; }
	void Set_AABB(const AABB& t) { m_tAABB = t; }

	void Set_Layer(COLLAYER Layer) { m_Layer = Layer; }
	COLLAYER Get_Layer() const { return m_Layer; }

	void Set_Trigger(_bool b) { m_bTrigger = b; }
	_bool Get_Trigger() const { return m_bTrigger; }

	using HIT_CALLBACK = function<void(CBase* pOwner, CBase* pOther)>;
	void Set_HitCallback(HIT_CALLBACK cb) { m_Callback = cb; }

public:
	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, const AABB& tInitAABB, COLLAYER iLayer = CL_NONE, _bool bTrigger = false);
	virtual CComponent* Clone() override;

private:
	AABB			m_tAABB;
	COLLAYER		m_Layer;
	_bool			m_bTrigger;
	CBase*			m_pOwner;
	HIT_CALLBACK	m_Callback;

private:
	virtual void Free() override;
};

END