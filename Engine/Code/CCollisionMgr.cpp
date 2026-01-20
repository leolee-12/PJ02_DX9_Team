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
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_MONSTER });		// 어떤 그룹끼리 충돌체크할 것인지 등록
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_ITEM});
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_GRASS });
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_WARP });
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_WARP });
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_MBULLET });
	m_vecCollisionPool.push_back({ CL_MONSTER, CL_GRASS });
	m_vecCollisionPool.push_back({ CL_MONSTER, CL_PBULLET });
	// 윤석현 추가
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_TRIGGER });
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_BORDER });
	m_vecCollisionPool.push_back({ CL_MONSTER, CL_BORDER });
	m_vecCollisionPool.push_back({ CL_PLAYER, CL_CHEST });
	// WY
	m_vecCollisionPool.push_back({ CL_NPC, CL_BORDER });

	return S_OK;
}

void CCollisionMgr::Check_Collisions(const _float& fDeltaTime)
{
	for (auto& vec : m_vecCollisionPool)									// 등록된 그룹풀 전체순회
	{
		for (auto& Group1 : m_hmapCollisionGroup[vec.first]) {				// 첫번째그룹
			for (auto& Group2 : m_hmapCollisionGroup[vec.second]) {			// 두번째그룹
				if (IntersectAABB(Group1.tAABB, Group2.tAABB))				// 2중 for 돌면서 충돌체크
				{
					Group1.pOwner->OnCollision(Group2.pOwner);				// 충돌 했다면 상대방 게임 포인터로 콜백메서드 호출
					Group2.pOwner->OnCollision(Group1.pOwner);				// 매니저는 충돌을 감지하고 했다는 사실만 객체에게 알림
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

	auto [iter, inserted] = m_hmapCollisionGroup.try_emplace(Group);  // 해당 키값이 있는지 없는지 확인후 없으면 생성, 있으면 아무동작안함

	if (inserted) {									// 삽입시 (키값이 없으면)
		iter->second.push_back({ pOwner, aabb });	// 매개변수로 받는 정보를 등록
		pOwner->AddRef();							// 등록 후 레퍼런스카운트 증가
		return;
	}

													// 삽입이 이루어지지않으면 (이미 키값그룹이 존재하면)
	for (auto& colliderinfo : iter->second)			// 해당 키값에 대한 전체순회
	{
		if (colliderinfo.pOwner == pOwner) {		// 순회중 등록되어있는 게임 포인터가 매개변수로 들어온포인터와 같으면
			colliderinfo.tAABB.x = aabb.x;			// AABB 중심값 갱신
			colliderinfo.tAABB.y = aabb.y;
			colliderinfo.tAABB.z = aabb.z;
			colliderinfo.tAABB.hx = aabb.hx;
			colliderinfo.tAABB.hy = aabb.hy;
			colliderinfo.tAABB.hz = aabb.hz;
			return;
		}
	}
													// 키값은 있는데 매개변수로 들어온 게임포인터로 등록된 AABB가 없다면
	iter->second.push_back({ pOwner, aabb });		// 해당 그룹에 정보를 추가 후
	pOwner->AddRef();								// 레퍼런스카운트 증가
}

inline void CCollisionMgr::Reset_For_SceneChange()
{
	Free();
}

void CCollisionMgr::Push_State()
{
	// 현재 상태를 스택에 보관 (Release 안 함!)
	m_stackState.push(m_hmapCollisionGroup);

	// 새 씬용으로 비움
	m_hmapCollisionGroup.clear();
	m_vecReleaseQueue.clear();
	m_vecUnregisterRequestQueue.clear();
}

void CCollisionMgr::Pop_State()
{
	if (m_stackState.empty())
		return;

	// 현재 상태 정리 (미니게임 콜라이더 등)
	Free();

	// 보관된 상태 복원
	m_hmapCollisionGroup = m_stackState.top();
	m_stackState.pop();
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
	// remove_if < 알고리즘 한번 찾아보세요.
	auto removeiter = remove_if(vec.begin(), vec.end(), [pOwner, this](const COLINFO& colinfo) {
		if (colinfo.pOwner == pOwner) {
			m_vecReleaseQueue.push_back(colinfo.pOwner);			// 우리가 순회중에 객체를 삭제하면 객체의 Free에서 매니저를 다시 호출 할 수도 있음
			return true;											// 순회중에 삭제를 할 경우에 이미 순회/삭제 중인 컨테이너의 요소에 접근 할 수 있어서 반복자 무효화 발생할 수 있음
		}															// 해당하는 가능성을 제거하기위해 순회중에 삭제큐에 담아뒀다가 순회가 끝나면 한번에 삭제처리
		return false;
		});
	// 이건 다른 얘기긴한데 remove_if 알고리즘 아시면 도움이 됩니다용

	if (removeiter != vec.end())
	{
		vec.erase(removeiter, vec.end());
	}
	// 삭제 큐를 여기서 비우지는 않는 이유:
	// Check_Collisions 에서 여기가 호출되면 컨테이너 순회 중 삭제가 발생하므로,
	// 안전하게 순회가 끝난(삭제 루프 밖)에서 ExecuteUnregister()로 처리한다.
}


vector<CGameObject*> CCollisionMgr::Test_AABB(const AABB& aabb, COLGROUP Groupflag)
{
	vector<CGameObject*> vecTemp;			// 반환용 임시 벡터

	for (auto& pair : m_hmapCollisionGroup)	// 충돌 그룹 전체순회
	{
		COLGROUP group = pair.first;		// 충돌 그룹데이터 추출 (플래그와 비트연산을 위해)

		// 요청한 레이어 그룹 해당 그룹이 포함되어 있지 않으면 건너뜀
		if ((Groupflag & group) == 0)		// 비트 마스킹
			continue;

		for (const auto& colliderinfo : pair.second)		// 요청한 그룹 플래그에 해당 그룹이 포함되어있다면,
		{													// 해당 그룹 전체 순회
			if (IntersectAABB(aabb, colliderinfo.tAABB))	// 충돌체크
			{
				vecTemp.push_back(colliderinfo.pOwner);		// 충돌했다면 충돌한 대상 게임포인터를 임시 벡터에 등록
			}
		}
	}

	return vecTemp;			// 충돌한 모든 충돌체의 게임포인터를 반환
}

void CCollisionMgr::Free()
{
	// 전체순회하면서 콜라이더정보, 게임포인터 삭제
	for (auto& pair : m_hmapCollisionGroup) {
		for (auto& colinfo : pair.second) {
			m_vecReleaseQueue.push_back(colinfo.pOwner);  // 우리가 순회중에 객체를 삭제하면 객체의 Free에서 매니저를 다시 호출 할 수도 있음
			colinfo.pOwner = nullptr;					  // 순회중에 삭제를 할 경우에 이미 순회/삭제 중인 컨테이너의 요소에 접근 할 수 있어서 반복자 무효화 발생할 수 있음
		}												  // 해당하는 가능성을 제거하기위해 순회중에 삭제큐에 담아뒀다가 순회가 끝나면 한번에 삭제처리
		pair.second.clear();
	}
	m_hmapCollisionGroup.clear();

	Execute_Release();
}
