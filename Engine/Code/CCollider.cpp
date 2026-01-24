#include "CCollider.h"
#include "CTransform.h"
#include "CCollisionMgr.h"
#include "CGameObject.h"
#include "CRenderer.h"



CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_tAABB{ 0,0,0, 0.5f,0.5f,0.5f }, m_pOwner(nullptr), m_Group(CL_NONE)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs), m_tAABB(rhs.m_tAABB), m_pOwner(nullptr), m_Group(rhs.m_Group)
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

	// 매니저에 갱신: 등록된 소유자가 있으면 매니저에 갱신 전달
	if (m_pOwner)
		Engine::CCollisionMgr::GetInstance()->RegisterCollider(m_pOwner, m_tAABB, m_Group);
}

void CCollider::UpdateFromCustom(const AABB& tAABB)
{
	// 매니저에 갱신: 등록된 소유자가 있으면 매니저에 갱신 전달
	if (m_pOwner)
		Engine::CCollisionMgr::GetInstance()->RegisterCollider(m_pOwner, tAABB, m_Group);
}

void CCollider::RegisterToManager(CGameObject* pOwner, COLGROUP Layerflag)
{
	// 소유자 포인터는 매니저가 AddRef/Release 관리를 담당함
	// 소유권은 매니저 or 컴포넌트 둘중하나가 책임
	// 중앙관리자인 매니저가 총괄하는걸 선택함
	if (pOwner == nullptr) { return; }
	m_pOwner = pOwner;
	m_Group = Layerflag;

	Engine::CCollisionMgr::GetInstance()->RegisterCollider(m_pOwner, m_tAABB, m_Group);
}

void CCollider::UnregisterFromManager()
{
	if (m_pOwner)
	{
		Engine::CCollisionMgr::GetInstance()->RequestUnregister(m_pOwner, m_Group);
	}
}

void CCollider::Update_AABBforRender()
{
	CRenderer::GetInstance()->Add_ColliderDebugGroup(this);
}

void CCollider::Render_Collider()
{
	if (!m_pGraphicDev) return; 
	// AABB 정보에서 center와 half-size 가져오기 
	D3DXVECTOR3 c = { m_tAABB.x, m_tAABB.y, m_tAABB.z };
	D3DXVECTOR3 h = { m_tAABB.hx, m_tAABB.hy, m_tAABB.hz };
	// 8개 꼭짓점 계산 
	D3DXVECTOR3 v[8] = { 
		{c.x - h.x, c.y - h.y, c.z - h.z}, 
		{c.x - h.x, c.y - h.y, c.z + h.z}, 
		{c.x - h.x, c.y + h.y, c.z - h.z}, 
		{c.x - h.x, c.y + h.y, c.z + h.z}, 
		{c.x + h.x, c.y - h.y, c.z - h.z}, 
		{c.x + h.x, c.y - h.y, c.z + h.z}, 
		{c.x + h.x, c.y + h.y, c.z - h.z}, 
		{c.x + h.x, c.y + h.y, c.z + h.z}, 
	}; 
	// 12개 엣지를 연결하는 인덱스 
	WORD indices[24] = 
	{ 
		0,1, 0,2, 0,4, 
		7,6, 7,5, 7,3, 
		1,5, 1,3, 2,3, 
		2,6, 4,5, 4,6 
	}; 

	DebugVertex verts[8]; 
	for (int i = 0; i < 8; ++i) 
	{ 
		verts[i].vPosition = v[i]; 
		verts[i].dwColor = D3DCOLOR_ARGB(255, 0, 255, 0); 
	} 

	DWORD dwLighting;
	m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetTexture(0, nullptr);

	m_pGraphicDev->SetFVF(FVF_DEBUG);
	m_pGraphicDev->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 8, 12, indices, D3DFMT_INDEX16, verts, sizeof(DebugVertex) );

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwLighting);
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
