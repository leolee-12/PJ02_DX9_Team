#include "CCollider.h"
#include "CTransform.h"
#include "CCollisionMgr.h"



CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_tAABB{ 0,0,0, 0.5f,0.5f,0.5f }, m_Layer(CL_NONE), m_bTrigger(false), m_pOwner(nullptr)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs), m_tAABB(rhs.m_tAABB), m_Layer(rhs.m_Layer), m_bTrigger(rhs.m_bTrigger), m_pOwner(nullptr), m_Callback(rhs.m_Callback)
{
}

CCollider::~CCollider()
{
}

HRESULT CCollider::Ready_Collider(const AABB& tInitAABB, COLLAYER Layer, _bool bTrigger)
{
	m_tAABB = tInitAABB;
	m_Layer = Layer;
	m_bTrigger = bTrigger;
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
		Engine::CCollisionMgr::GetInstance()->RegisterCollider(m_pOwner, m_tAABB, m_Layer, m_Callback);
}

void CCollider::RegisterToManager(CBase* pOwner)
{
	// 소유자 포인터는 매니저가 AddRef/Release 관리를 담당함
	m_pOwner = pOwner;
	Engine::CCollisionMgr::GetInstance()->RegisterCollider(pOwner, m_tAABB, m_Layer, m_Callback);
}

void CCollider::UnregisterFromManager()
{
	if (m_pOwner)
	{
		Engine::CCollisionMgr::GetInstance()->UnregisterCollider(m_pOwner);
		m_pOwner = nullptr;
	}
}

CCollider* CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, const AABB& tInitAABB, COLLAYER Layer, _bool bTrigger)
{
	CCollider* pInstance = new CCollider(pGraphicDev);
	if (FAILED(pInstance->Ready_Collider(tInitAABB, Layer, bTrigger)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CComponent* CCollider::Clone()
{
	CCollider* pClone = new CCollider(*this);
	pClone->m_pGraphicDev = m_pGraphicDev;
	pClone->m_pOwner = nullptr;
	pClone->m_Callback = nullptr;
	return pClone;
}

void CCollider::Free()
{
	UnregisterFromManager();
	CComponent::Free();
}