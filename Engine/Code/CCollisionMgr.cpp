#include "CCollisionMgr.h"
#include "CGameObject.h"


IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
	: m_bRoof(false)
{
}

CCollisionMgr::~CCollisionMgr()
{
	Free();
}

HRESULT CCollisionMgr::Ready_CollisionMgr()
{
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_MONSTER });
	return S_OK;
}

void CCollisionMgr::Check_Collisions(const _float& fDeltaTime)
{
	for (auto& vec : m_vecCollisionPool)
	{
		for (auto& Layer1 : m_hmapCollisionLayer[vec.first]) {
			for (auto& Layer2 : m_hmapCollisionLayer[vec.second]) {
				if (IntersectAABB(Layer1.tAABB, Layer2.tAABB))
				{
					Layer1.pOwner->OnCollision(Layer2.pOwner);
					Layer2.pOwner->OnCollision(Layer1.pOwner);
				}
			}
		}
	}
}

void CCollisionMgr::RegisterCollider(CGameObject* pOwner, const AABB& aabb, COLLAYER Layer)
{
	if (pOwner == nullptr) { return; }

	auto [iter, inserted] = m_hmapCollisionLayer.try_emplace(Layer);
	
	if (inserted) {
		iter->second.push_back({ aabb, pOwner });
		pOwner->AddRef();
		return;
	}

	for (auto& vec : iter->second)
	{
		if (vec.pOwner == pOwner) {
			vec.tAABB.x = aabb.x;
			vec.tAABB.y = aabb.y;
			vec.tAABB.z = aabb.z;
			return;
		}
	}

	iter->second.push_back({ aabb, pOwner });
	pOwner->AddRef();
}

void CCollisionMgr::UnregisterCollider(CGameObject* pOwner, COLLAYER Layer)
{
	if (pOwner == nullptr) { return; }
	auto iter = m_hmapCollisionLayer.find(Layer);
	if (iter == m_hmapCollisionLayer.end()) { return; }

	auto& vec = iter->second;
	auto removeiter = remove_if(vec.begin(), vec.end(), [pOwner](const COLINFO& colinfo) {
		return colinfo.pOwner == pOwner;
		});

	if (removeiter != vec.end())
	{
		for (auto it = removeiter; it != vec.end(); ++it)
		{
			Safe_Release(it->pOwner);
		}
		vec.erase(removeiter, vec.end());
	}
}


vector<CGameObject*> CCollisionMgr::Query_AABB(const AABB& aabb, COLLAYER Layerflag)
{
	vector<CGameObject*> vecTemp;

	for (auto& pair : m_hmapCollisionLayer)
	{
		COLLAYER layer = pair.first;

		// 요청한 레이어 플래그에 해당 레이어가 포함되어 있지 않으면 건너뜀
		if ((Layerflag & layer) == 0)
			continue;

		for (const auto& colinfo : pair.second)
		{
			if (IntersectAABB(aabb, colinfo.tAABB))
			{
				vecTemp.push_back(colinfo.pOwner);
			}
		}
	}

	return vecTemp;
}

void CCollisionMgr::Free()
{
	for (auto iter = m_hmapCollisionLayer.begin();
		iter != m_hmapCollisionLayer.end();
		++iter)
	{
		for (auto& vec : iter->second)
		{
			Safe_Release(vec.pOwner);
		}
		iter->second.clear();
	}
	m_hmapCollisionLayer.clear();
}