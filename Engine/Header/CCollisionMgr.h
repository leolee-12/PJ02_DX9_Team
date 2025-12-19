#pragma once

#include "CBase.h"
#include "Engine_Define.h"

//-----------------------------------
//	충돌 처리 담당 매니저 클래스입니다.
//	콜라이더들을 해당 매니저에서 모아서 관리합니다.
//	충돌처리는 모두 여기서 합니다.
//-----------------------------------



BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CCollisionMgr : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	explicit CCollisionMgr();
	virtual ~CCollisionMgr();

public:
	HRESULT Ready_CollisionMgr();							// 셋업함수
	void Check_Collisions(const _float& fDeltaTime);		// 매프레임 충돌체크 함수

	void RegisterCollider(CGameObject* pOwner, const AABB& aabb, COLGROUP Group);	// 콜라이더 등록/갱신 함수
	void UnregisterCollider(CGameObject* pOwner, COLGROUP Group);					// 콜라이더 해제/삭제 함수

	vector<CGameObject*> Query_AABB(const AABB& aabb, COLGROUP Groupflag = CL_ALL);	// 임시 충돌체크함수 (반환값 충돌한 모든 충돌체의 오너포인터 집단)

private:

	static bool IntersectAABB(const AABB& a, const AABB& b)			// private지만 static을 붙혀서 헬퍼함수라는걸 명시함
	{
		// 충돌검사 AABB 
		if (fabsf(a.x - b.x) > (a.hx + b.hx)) { return false; }		// A,B의 X축 중심기준으로 떨어진 거리가 X축 halfsize 보다 크면 = 겹치지않음.
		if (fabsf(a.y - b.y) > (a.hy + b.hy)) { return false; }		// A,B의 Y축 중심기준으로 떨어진 거리가 Y축 halfsize 보다 크면 = 겹치지않음.
		if (fabsf(a.z - b.z) > (a.hz + b.hz)) { return false; }		// A,B의 Z축 중심기준으로 떨어진 거리가 Z축 halfsize 보다 크면 = 겹치지않음.
		return true;												// 모든축기 겹친다면 = 충돌!
	}

	typedef struct tagCollisionInfo			// 충돌체의 정보를 담을 매니저에서만 쓸 구조체
	{	
		AABB			tAABB;				// AABB 정보
		CGameObject*	pOwner;				// 충돌체 오너(소유자)
	}COLINFO;

	unordered_map<COLGROUP, vector<COLINFO>> m_hmapCollisionGroup;		// 충돌체데이터 집합 (그룹별)
	vector<pair<COLGROUP, COLGROUP>> m_vecCollisionPool;				// 어떤그룹과 어떤그룹이 충돌하게할지 정하는 풀
	_bool		m_bRoof;												// 컨테이너 순회중에 삽입삭제 방지용 변수인데 지금은 안씀

private:
	virtual void Free() override;
};

END