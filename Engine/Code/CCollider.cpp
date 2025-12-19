#include "CCollider.h"
#include "CTransform.h"
#include "CCollisionMgr.h"
#include "CGameObject.h"



CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_tAABB{ 0,0,0, 0.5f,0.5f,0.5f }, m_pOwner(nullptr), m_Layer(CL_NONE)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs), m_tAABB(rhs.m_tAABB), m_pOwner(nullptr), m_Layer(rhs.m_Layer)
{
}

CCollider::~CCollider()
{
}

HRESULT CCollider::Ready_Collider(optional<AABB> tInitAABB)
{
	if (tInitAABB.has_value()) {
		m_tAABB = tInitAABB.value();
	}
	return S_OK;
}

void CCollider::UpdateFromTransform(CTransform* pTransform)
{
	if (nullptr == pTransform)
		return;

	_vec3 vPos{};
	pTransform->Get_Info(INFO_POS, &vPos);

	m_tAABB.x = vPos.x;
	m_tAABB.y = vPos.y;
	m_tAABB.z = vPos.z;

	// 매니저에 갱신: 등록된 소유자가 있으면 매니저에 갱신 전달(매니저가 내부에서 기존 엔트리 업데이트)
	if (m_pOwner)
		Engine::CCollisionMgr::GetInstance()->RegisterCollider(m_pOwner, m_tAABB, m_Layer);
}

void CCollider::RegisterToManager(CGameObject* pOwner, COLLAYER Layerflag)
{
	// 소유자 포인터는 매니저가 AddRef/Release 관리를 담당함
	if (pOwner == nullptr) { return; }
	m_pOwner = pOwner;
	//m_pOwner->AddRef();
	m_Layer = Layerflag;

	Engine::CCollisionMgr::GetInstance()->RegisterCollider(m_pOwner, m_tAABB, m_Layer);
}

void CCollider::UnregisterFromManager()
{
	if (m_pOwner)
	{
		Engine::CCollisionMgr::GetInstance()->UnregisterCollider(m_pOwner, m_Layer);
		//Safe_Release(m_pOwner);
	}
}

CCollider* CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, optional<AABB> tInitAABB)
{
	CCollider* pInstance = new CCollider(pGraphicDev);
	if (FAILED(pInstance->Ready_Collider(tInitAABB)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CComponent* CCollider::Clone()
{
	CCollider* pClone = new CCollider(*this);
	return pClone;
}

void CCollider::Free()
{
	UnregisterFromManager();
	CComponent::Free();
}