#pragma once

#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CCollisionMgr : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	explicit CCollisionMgr();
	virtual ~CCollisionMgr();

public:
	HRESULT Ready_CollisionMgr();
	void Check_Collisions(const _float& fDeltaTime);

	void RegisterCollider(CGameObject* pOwner, const AABB& aabb, COLLAYER Layer);
	void UnregisterCollider(CGameObject* pOwner, COLLAYER Layer);

	vector<CGameObject*> Query_AABB(const AABB& aabb, COLLAYER Layerflag = CL_ALL);

private:

	static bool IntersectAABB(const AABB& a, const AABB& b)
	{
		// 충돌검사 AABB 
		if (fabsf(a.x - b.x) > (a.hx + b.hx)) { return false; }		// A,B의 X축 중심기준으로 떨어진 거리가 X축 halfsize 보다 크면 = 겹치지않음.
		if (fabsf(a.y - b.y) > (a.hy + b.hy)) { return false; }		// A,B의 Y축 중심기준으로 떨어진 거리가 Y축 halfsize 보다 크면 = 겹치지않음.
		if (fabsf(a.z - b.z) > (a.hz + b.hz)) { return false; }		// A,B의 Z축 중심기준으로 떨어진 거리가 Z축 halfsize 보다 크면 = 겹치지않음.
		return true;												// 모든축기 겹친다면 = 충돌!
	}

	typedef struct tagCollisionInfo 
	{
		AABB tAABB;
		CGameObject* pOwner;
	}COLINFO;

	unordered_map<COLLAYER, vector<COLINFO>> m_hmapCollisionLayer;
	vector<pair<COLLAYER, COLLAYER>> m_vecCollisionPool;
	_bool		m_bRoof;

private:
	virtual void Free() override;
};

END