#include "pch.h"
#include "CMapBorder.h"
#include "CProtoMgr.h"
#include "Engine_Enum.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

CMapBorder::CMapBorder(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_iTileX(0)
	, m_iTileZ(0)
{
	m_eOBJID = OID_BORDER;
}

CMapBorder::~CMapBorder()
{
}

HRESULT CMapBorder::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	//m_pColliderCom->RegisterToManager(this, CL_BORDER);

	return S_OK;
}

_int CMapBorder::Update_GameObject(const _float& fTimeDelta)
{
	//m_pColliderCom->UpdateFromTransform(m_pTransformCom);
	const float TILE_SIZE = 8.0f;
	float worldX = m_iTileX * TILE_SIZE + TILE_SIZE * 0.5f;
	float worldZ = m_iTileZ * TILE_SIZE + TILE_SIZE * 0.5f;
	Engine::AABB tAABB;
	tAABB.x = worldX;
	tAABB.y = 1.f;
	tAABB.z = worldZ;
	tAABB.hx = TILE_SIZE * 0.5f;
	tAABB.hy = 2.f;
	tAABB.hz = TILE_SIZE * 0.5f;
	m_pColliderCom->Set_AABB(tAABB);
	m_pColliderCom->UpdateFromCustom(tAABB);

	if (g_bDebug) m_pColliderCom->Update_AABBforRender();

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);


	// Debug mode only - render collider bo
	//if (g_bDebug)
	//	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CMapBorder::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMapBorder::Render_GameObject()
{
	if (g_bDebug)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
		m_pColliderCom->Render_Collider();
	}
}

void CMapBorder::Set_TilePosition(_int tileX, _int tileZ)
{
	m_iTileX = tileX;
	m_iTileZ = tileZ;

	const float TILE_SIZE = 8.0f;
	float worldX = tileX * TILE_SIZE + TILE_SIZE * 0.5f;
	float worldZ = tileZ * TILE_SIZE + TILE_SIZE * 0.5f;

	m_pTransformCom->Set_Pos(worldX, 0.f, worldZ);

	Engine::AABB aabb;
	aabb.x = worldX;
	aabb.y = 1.f;
	aabb.z = worldZ;
	aabb.hx = TILE_SIZE * 0.5f;
	aabb.hy = 2.f;
	aabb.hz = TILE_SIZE * 0.5f;
	m_pColliderCom->Set_AABB(aabb);

	m_pColliderCom->RegisterToManager(this, CL_BORDER);
}

HRESULT CMapBorder::Add_Component()
{
	CComponent* pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>
		(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<CCollider*>
		(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

CMapBorder* CMapBorder::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int tileX, _int tileZ)
{
	CMapBorder* pInstance = new CMapBorder(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}

	pInstance->Set_TilePosition(tileX, tileZ);

	return pInstance;
}

void CMapBorder::Free()
{
	CGameObject::Free();
}
