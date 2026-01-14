#pragma once

#include "CComponent.h"
#include "Engine_Define.h"

//-----------------------------------
//	충돌체 담당 클래스입니다.
//	충돌처리를 해야하는 객체들은
//	해당 컴포넌트 포함하면됩니다.
//-----------------------------------


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
	void Set_AABB(AABB tAABB) { m_tAABB = tAABB; }
	AABB Get_AABB() { return m_tAABB; }

	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, optional<AABB> tInitAABB = nullopt);
	CComponent* Clone() override;

	void UpdateFromTransform(CTransform* pTransform);				// 충돌체위치 갱신 함수
	void UpdateFromCustom(AABB& tAABB);								// 충돌체위치 갱신 함수
	void RegisterToManager(CGameObject* pOwner, COLGROUP Group);	// 매니저에 콜라이더 정보 등록 함수
	void UnregisterFromManager();									// 매니저에 콜라이더 정보 등록 해제함수

	void Update_AABBforRender();
	void Render_Collider();										// 디버그용 콜라이더 렌더링 함수


private:
	HRESULT Ready_Collider(optional<AABB> tInitAABB);  // 콜라이더 셋업함수

private:
	AABB		 m_tAABB;		// AABB용 충돌체 정보
	COLGROUP	 m_Group;		// 충돌처리 그룹 (매니저에서 그룹별로 충돌체크 하기 위함)
	CGameObject* m_pOwner;		// 충돌체 소유자 포인터 (예시. 플레이어의 충돌체 = 플레이어의 포인터)
private:
	virtual void Free() override;
};

END
