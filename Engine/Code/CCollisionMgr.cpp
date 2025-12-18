#include "CCollisionMgr.h"


IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
}

HRESULT CCollisionMgr::Ready_CollisionMgr()
{
	m_vColliders.clear();
	return S_OK;
}

void CCollisionMgr::Update(const _float& fDeltaTime)
{
	// 델타타임으로 특정시간마다 전체검사 같은 로직 구현 가능성있음
	// 현재는 Query 기반 사용 권장. 자동 검사 필요시 여기에 구현.
}

void CCollisionMgr::RegisterCollider(CBase* pOwner, const AABB& aabb, COLLAYER Layer, COLLISION_CALLBACK pCallback)
{
	for (auto& sc : m_vColliders)
	{
		if (sc.pOwner == pOwner)
		{
			sc.AABB = aabb;
			sc.Layer = Layer;
			sc.Callback = pCallback;
			return;
		}
	}

	COLLIDER s;
	s.pOwner = pOwner;
	s.AABB = aabb;
	s.Layer = Layer;
	s.Callback = pCallback;
	m_vColliders.push_back(s);
}

void CCollisionMgr::UnregisterCollider(CBase* pOwner)
{
	m_vColliders.erase(
		remove_if(m_vColliders.begin(), m_vColliders.end(), [&](const COLLIDER& s) { return s.pOwner == pOwner; }),
		m_vColliders.end());
}

vector<CBase*> CCollisionMgr::Query_AABB(const AABB& aabb, COLLAYER Layer)
{
	vector<CBase*> result;
	for (const auto& sc : m_vColliders)
	{
		if (Layer != CL_ALL && sc.Layer != Layer)
			continue;

		if (IntersectAABB(sc.AABB, aabb))
			result.push_back(sc.pOwner);
	}
	return result;
}

void CCollisionMgr::Free()
{
	m_vColliders.clear();
}