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
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_MONSTER });		// � �׷쳢�� �浹üũ�� ���� ���
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_GRASS });   
	m_vecCollisionPool.push_back({ CL_MONSTER, CL_GRASS });
	return S_OK;
}

void CCollisionMgr::Check_Collisions(const _float& fDeltaTime)
{
	for (auto& vec : m_vecCollisionPool)									// ��ϵ� �׷�Ǯ ��ü��ȸ
	{
		for (auto& Group1 : m_hmapCollisionGroup[vec.first]) {				// ù��°�׷�
			for (auto& Group2 : m_hmapCollisionGroup[vec.second]) {			// �ι�°�׷� 
				if (IntersectAABB(Group1.tAABB, Group2.tAABB))				// 2�� for ������ �浹üũ
				{
					Group1.pOwner->OnCollision(Group2.pOwner);				// �浹 �ߴٸ� ������ ���� �����ͷ� ���ݸ��� ȣ��
					Group2.pOwner->OnCollision(Group1.pOwner);				// �Ŵ����� �浹�� �����ϰ� �ߴ��� ������ ��ü���� �˸�
				}
			}
		}
	}

	Execute_UnregisterRequests();
	Execute_Release();
}

void CCollisionMgr::RegisterCollider(CGameObject* pOwner, const AABB& aabb, COLGROUP Group)
{
	if (pOwner == nullptr) { return; }

	auto [iter, inserted] = m_hmapCollisionGroup.try_emplace(Group);  // �ش� Ű���� �ִ��� ������ Ȯ���� ������ ����, ������ �ƹ��ൿ������
	
	if (inserted) {									// ���Խ� (Ű���� ������)
		iter->second.push_back({ pOwner, aabb });	// �Ű������� �޴� ������ ����
		pOwner->AddRef();							// ���� ����ī��Ʈ ����
		return;										
	}
	
													// ������ �̷����������� (�̹� Ű���׷��� �����ϸ�)				
	for (auto& colliderinfo : iter->second)			// �ش� Ű���� ���� ��ü��ȸ
	{
		if (colliderinfo.pOwner == pOwner) {		// ��ȸ�� ����Ǿ��ִ� ���� �����Ͱ� �Ű������� ���������Ϳ� ������
			colliderinfo.tAABB.x = aabb.x;			// AABB �߽��� ����
			colliderinfo.tAABB.y = aabb.y;
			colliderinfo.tAABB.z = aabb.z;
			return;
		}
	}
													// Ű���� �ִµ� �Ű������� ���� ���������ͷ� ��ϵ� AABB�� ���ٸ�
	iter->second.push_back({ pOwner, aabb });		// �ش� �׷쿡 ������ �߰� ��
	pOwner->AddRef();								// ����ī��Ʈ ����
}

inline void CCollisionMgr::Reset_For_SceneChange()
{
	Free();
}

void CCollisionMgr::RequestUnregister(CGameObject* pOwner, COLGROUP Group)
{
	if (pOwner == nullptr) { return; }
	m_vecUnregisterRequestQueue.push_back({ pOwner, Group });
}

void CCollisionMgr::Execute_UnregisterRequests()
{
	if (m_vecUnregisterRequestQueue.empty()) { return; }
	for (auto& requestinfo : m_vecUnregisterRequestQueue)
	{
		UnregisterCollider(requestinfo.pOwner, requestinfo.eGroup);
	}
	m_vecUnregisterRequestQueue.clear();
}

void CCollisionMgr::Execute_Release()
{
	if (m_vecReleaseQueue.empty()) { return; }
	for (auto& pOwner : m_vecReleaseQueue)
	{
		Safe_Release(pOwner);
	}
	m_vecReleaseQueue.clear();
}

void CCollisionMgr::UnregisterCollider(CGameObject* pOwner, COLGROUP Group)
{
	if (pOwner == nullptr) { return; }
	auto iter = m_hmapCollisionGroup.find(Group);
	if (iter == m_hmapCollisionGroup.end()) { return; }

	auto& vec = iter->second;
	// remove_if < �˰���� �� ã�ƺ�����.
	auto removeiter = remove_if(vec.begin(), vec.end(), [pOwner, this](const COLINFO& colinfo) {
		if (colinfo.pOwner == pOwner) {
			m_vecReleaseQueue.push_back(colinfo.pOwner);			// �츮�� ��ȸ�߿� ��ü�� �����ϸ� ��ü�� Free���� �Ŵ����� �ٽ� ȣ�� �� ���� ����
			return true;											// ��ȸ�߿� ������ �� ������ �̹� ��ȸ/���� ���� �����̳��� ��ҿ� ���� �� �� �־ �ݺ��� ��ȿȭ �߻��� �� ����
		}															// �ش��ϴ� ���ɼ��� �����ϱ����� ��ȸ�߿� ����ť�� ��Ƶ״ٰ� ��ȸ�� ������ �ѹ��� ����ó��
		return false;
		});
	// ���� ���� ������ remove_if �˰���� �ƽø� ���ذ� �˴ϴٿ�
	
	if (removeiter != vec.end())
	{
		vec.erase(removeiter, vec.end());
	}
	// ���� ť�� ���⼭ �������� �ʴ� ����:
	// Check_Collisions ���� ���� ȣ��Ǹ� �����̳� ��ȸ �� ������ �߻��ϹǷ�,
	// �����ϰ� ������ ��(���� ���� ����)���� ExecuteUnregister()�� ó���Ѵ�.
}


vector<CGameObject*> CCollisionMgr::Test_AABB(const AABB& aabb, COLGROUP Groupflag)
{
	vector<CGameObject*> vecTemp;			// ��ȯ�� �ӽ� ����

	for (auto& pair : m_hmapCollisionGroup)	// �浹 �׷� ��ü��ȸ
	{
		COLGROUP group = pair.first;		// �浹 �׷쵥���� ���� (�÷����� ��Ʈ������ ����)

		// ��û�� ���̾� �׷� �ش� �׷��� ���ԵǾ� ���� ������ �ǳʶ�
		if ((Groupflag & group) == 0)		// ��Ʈ ����ŷ
			continue;

		for (const auto& colliderinfo : pair.second)		// ��û�� �׷� �÷��׿� �ش� �׷��� ���ԵǾ��ִٸ�,
		{													// �ش� �׷� ��ü ��ȸ
			if (IntersectAABB(aabb, colliderinfo.tAABB))	// �浹üũ
			{
				vecTemp.push_back(colliderinfo.pOwner);		// �浹�ߴٸ� �浹�� ��� ���������͸� �ӽ� ���Ϳ� ����
			}
		}
	}

	return vecTemp;			// ������ ��� �浹ü�� ���������͸� ��ȯ
}

void CCollisionMgr::Free()
{
	// ��ü��ȸ�ϸ鼭 �ݶ��̴�����, ���������� ����
	for (auto& pair : m_hmapCollisionGroup) {
		for (auto& colinfo : pair.second) {
			m_vecReleaseQueue.push_back(colinfo.pOwner);  // �츮�� ��ȸ�߿� ��ü�� �����ϸ� ��ü�� Free���� �Ŵ����� �ٽ� ȣ�� �� ���� ����
			colinfo.pOwner = nullptr;					  // ��ȸ�߿� ������ �� ������ �̹� ��ȸ/���� ���� �����̳��� ��ҿ� ���� �� �� �־ �ݺ��� ��ȿȭ �߻��� �� ����
		}												  // �ش��ϴ� ���ɼ��� �����ϱ����� ��ȸ�߿� ����ť�� ��Ƶ״ٰ� ��ȸ�� ������ �ѹ��� ����ó��
		pair.second.clear();
	}
	m_hmapCollisionGroup.clear();

	Execute_Release();
}