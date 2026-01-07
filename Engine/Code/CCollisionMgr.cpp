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
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_MONSTER });		// 어떤 그룹끼리 충돌체크를 할지 등록
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_GRASS });    // 추가
	m_vecCollisionPool.push_back({ CL_MONSTER, CL_GRASS });   // 추가
	return S_OK;
}

void CCollisionMgr::Check_Collisions(const _float& fDeltaTime)
{
	for (auto& vec : m_vecCollisionPool)									// 등록된 그룹풀 전체순회
	{
		for (auto& Group1 : m_hmapCollisionGroup[vec.first]) {				// 첫번째그룹
			for (auto& Group2 : m_hmapCollisionGroup[vec.second]) {			// 두번째그룹 
				if (IntersectAABB(Group1.tAABB, Group2.tAABB))				// 2중 for 문으로 충돌체크
				{
					Group1.pOwner->OnCollision(Group2.pOwner);				// 충돌 했다면 각자의 오너 포인터로 온콜리전 호출
					Group2.pOwner->OnCollision(Group1.pOwner);				// 매니저는 충돌을 누구하고 했는지 까지만 객체에게 알림
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

	auto [iter, inserted] = m_hmapCollisionGroup.try_emplace(Group);  // 해당 키값이 있는지 없는지 확인후 없으면 삽입, 있으면 아무행동도안함
	
	if (inserted) {									// 삽입시 (키값이 없을시)
		iter->second.push_back({ pOwner, aabb });	// 매개변수로 받는 데이터 저장
		pOwner->AddRef();							// 이후 참조카운트 증가
		return;										
	}
	
													// 삽입이 이뤄지지않으면 (이미 키값그룹이 존재하면)				
	for (auto& colliderinfo : iter->second)			// 해당 키값의 벡터 전체순회
	{
		if (colliderinfo.pOwner == pOwner) {		// 순회중 저장되어있는 오너 포인터가 매개변수의 오너포인터와 같으면
			colliderinfo.tAABB.x = aabb.x;			// AABB 중심점 갱신
			colliderinfo.tAABB.y = aabb.y;
			colliderinfo.tAABB.z = aabb.z;
			return;
		}
	}
													// 키값이 있는데 매개변수로 들어온 오너포인터로 등록된 AABB가 없다면
	iter->second.push_back({ pOwner, aabb });		// 해당 그룹에 데이터 추가 후
	pOwner->AddRef();								// 참조카운트 증가
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
	// remove_if < 알고리즘 함 찾아보세요.
	auto removeiter = remove_if(vec.begin(), vec.end(), [pOwner, this](const COLINFO& colinfo) {
		if (colinfo.pOwner == pOwner) {
			m_vecReleaseQueue.push_back(colinfo.pOwner);			// 우리가 순회중에 객체를 삭제하면 객체의 Free에서 매니저를 다시 호출 할 수도 있음
			return true;											// 순회중에 삭제가 또 들어오면 이미 순회/수정 중인 컨테이너의 요소에 접근 할 수 있어서 반복자 무효화 발생할 수 있음
		}															// 해당하는 가능성을 배제하기위해 순회중엔 삭제큐에 담아뒀다가 순회가 끝나면 한번에 삭제처리
		return false;
		});
	// 위에 삭제 과정은 remove_if 알고리즘 아시면 이해가 됩니다용
	
	if (removeiter != vec.end())
	{
		vec.erase(removeiter, vec.end());
	}
	// 삭제 큐를 여기서 실행하지 않는 이유:
	// Check_Collisions 루프 도중 호출되면 컨테이너 순회 중 삭제가 발생하므로,
	// 안전하게 프레임 끝(루프 종료 시점)에서 ExecuteUnregister()로 처리한다.
}


vector<CGameObject*> CCollisionMgr::Test_AABB(const AABB& aabb, COLGROUP Groupflag)
{
	vector<CGameObject*> vecTemp;			// 반환할 임시 벡터

	for (auto& pair : m_hmapCollisionGroup)	// 충돌 그룹 전체순회
	{
		COLGROUP group = pair.first;		// 충돌 그룹데이터 복사 (플래그의 비트연산을 위함)

		// 요청한 레이어 그룹 해당 그룹이 포함되어 있지 않으면 건너뜀
		if ((Groupflag & group) == 0)		// 비트 마스킹
			continue;

		for (const auto& colliderinfo : pair.second)		// 요청한 그룹 플래그에 해당 그룹이 포함되어있다면,
		{													// 해당 그룹 전체 순회
			if (IntersectAABB(aabb, colliderinfo.tAABB))	// 충돌체크
			{
				vecTemp.push_back(colliderinfo.pOwner);		// 충돌했다면 충돌한 대상 오너포인터를 임시 벡터에 저장
			}
		}
	}

	return vecTemp;			// 저장한 모든 충돌체의 오너포인터를 반환
}

void CCollisionMgr::Free()
{
	// 전체순회하면서 콜라이더정보, 오너포인터 제거
	for (auto& pair : m_hmapCollisionGroup) {
		for (auto& colinfo : pair.second) {
			m_vecReleaseQueue.push_back(colinfo.pOwner);  // 우리가 순회중에 객체를 삭제하면 객체의 Free에서 매니저를 다시 호출 할 수도 있음
			colinfo.pOwner = nullptr;					  // 순회중에 삭제가 또 들어오면 이미 순회/수정 중인 컨테이너의 요소에 접근 할 수 있어서 반복자 무효화 발생할 수 있음
		}												  // 해당하는 가능성을 배제하기위해 순회중엔 삭제큐에 담아뒀다가 순회가 끝나면 한번에 삭제처리
		pair.second.clear();
	}
	m_hmapCollisionGroup.clear();

	Execute_Release();
}