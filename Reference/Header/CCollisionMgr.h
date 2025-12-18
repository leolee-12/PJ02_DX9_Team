#pragma once

#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CCollisionMgr : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	explicit CCollisionMgr();
	virtual ~CCollisionMgr();

public:
	using COLLISION_CALLBACK = function<void(CBase* pA, CBase* pB)>;

	HRESULT Ready_CollisionMgr();
	void Update(const _float& fDeltaTime);

	void RegisterCollider(CBase* pOwner, const AABB& aabb, COLLAYER Layer, COLLISION_CALLBACK pCallback = nullptr);
	void UnregisterCollider(CBase* pOwner);

	vector<CBase*> Query_AABB(const AABB& aabb, COLLAYER Layer = CL_ALL);

private:
	typedef struct tagCollider
	{
		CBase*				pOwner;		// 충돌체의 소유자 포인터
		AABB				AABB;		// 충돌체위치, 사이즈 정보
		COLLAYER			Layer;		// 충돌체 레이어(타입)
		COLLISION_CALLBACK	Callback;	// 콜백함수 void(CBase*, CBase*);
	}COLLIDER;

	vector<COLLIDER> m_vColliders;

	static inline bool IntersectAABB(const AABB& a, const AABB& b)
	{
		// 충돌검사 AABB 
		if (fabsf(a.x - b.x) > (a.hx + b.hx)) { return false; }		// A,B의 X축 중심기준으로 떨어진 거리가 X축 halfsize 보다 크면 = 겹치지않음.
		if (fabsf(a.y - b.y) > (a.hy + b.hy)) { return false; }		// A,B의 Y축 중심기준으로 떨어진 거리가 Y축 halfsize 보다 크면 = 겹치지않음.
		if (fabsf(a.z - b.z) > (a.hz + b.hz)) { return false; }		// A,B의 Z축 중심기준으로 떨어진 거리가 Z축 halfsize 보다 크면 = 겹치지않음.
		return true;												// 모든축기 겹친다면 = 충돌!
	}

private:
	virtual void Free() override;
};

END