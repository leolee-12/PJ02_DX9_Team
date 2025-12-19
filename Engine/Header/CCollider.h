#pragma once

#include "CComponent.h"
#include "Engine_Define.h"



BEGIN(Engine)

class CGameObject;
class CTransform;

class ENGINE_DLL CCollider : public CComponent
{
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider();
	
public:
	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, optional<AABB> tInitAABB = nullopt);
	CComponent* Clone() override;

	void UpdateFromTransform(CTransform* pTransform);
	void RegisterToManager(CGameObject* pOwner, COLLAYER Layer);
	void UnregisterFromManager();


private:
	HRESULT Ready_Collider(optional<AABB> tInitAABB);

private:
	AABB		 m_tAABB;
	COLLAYER	 m_Layer;
	CGameObject* m_pOwner;
private:
	virtual void Free() override;
};

END