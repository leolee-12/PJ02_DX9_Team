#include "pch.h"
#include "CWarp.h"
#include "CCollider.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CMapWarp.h"

CWarp::CWarp(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
{

}

CWarp::CWarp(const CWarp& rhs)
	: CGameObject(rhs)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
{

}

CWarp::~CWarp()
{
}

HRESULT CWarp::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pColliderCom->RegisterToManager(this, CL_WARP);

	return S_OK;
}

_int CWarp::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	if (g_bDebug) { m_pColliderCom->Update_AABBforRender(); }

	return iExit;
}

void CWarp::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CWarp::Render_GameObject()
{
	if (!bRender) return;

	// 일단은 그냥 사각형 찍기 (바닥에)
	// collider comp에 맞춰서 찍어줘야 함
}

void CWarp::OnCollision(CGameObject* pObject)
{
	CWarp* pOtherWarp = Get_OtherWarp();

	_vec3 warpPos = pOtherWarp->Get_Pos();

	IMessageChannel::EVENT EWarp;
	EWarp.strType = L"Player.MapWarp";
	EWarp.eOBJID = Engine::OID_END;
	EWarp.hmapData.emplace(L"WarpPtr", this);
	m_pMessageChannel->Publish(EWarp);

}

void CWarp::Set_Position(float x, float z)
{
	m_pTransformCom->Set_Pos(x, 0.f, z);
}

void CWarp::Set_Scale(float size)
{
	// 정사각형이므로 사이즈 전부 같이 넣어주기
	m_pTransformCom->Set_Scale(size, size, size);

	Engine::AABB aabb;
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	aabb.x = vPos.x;
	aabb.y = vPos.y;
	aabb.z = vPos.z;
	aabb.hx = size * 0.5f; 
	aabb.hy = size * 0.5f;
	aabb.hz = size * 0.5f;
	m_pColliderCom->Set_AABB(aabb);
}

void CWarp::RegisterToManager()
{

	m_pColliderCom->RegisterToManager(this, CL_WARP);

}

CWarp* CWarp::Get_OtherWarp()
{
	if (MapWarpOuter == nullptr) return nullptr;

	return MapWarpOuter->Get_OtherWarp(this);
}

_vec3 CWarp::Get_Pos()
{
	_vec3 PosVec;
	m_pTransformCom->Get_Info(INFO_POS, &PosVec);
	
	return PosVec;
}

HRESULT CWarp::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;
	
	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });


	return S_OK;
}

CWarp* CWarp::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, CMapWarp* outer, _vec3 pos, WarpDir dir)
{
	CWarp* pWarp = new CWarp(pGraphicDev);


	pWarp->m_pMessageChannel = pMessageChannel;
	pWarp->m_pMessageChannel->AddRef();


	if (FAILED(pWarp->Ready_GameObject()))
	{
		Safe_Release(pWarp);
		MSG_BOX("CWarp Create Failed");
		return nullptr;
	}

	pWarp->Set_Outer(outer);
	pWarp->Set_WarpDir(dir);
	pWarp->Set_Position(pos.x, pos.z);
	pWarp->Set_Scale(10.f);

	_vec3 tmpPos = { pos.x, 0.f, pos.z };
	float posOffset = 10.0f;

	// 워프 포지션 조정.
	switch (dir)
	{
	case WARP_LEFT:
		tmpPos.x -= posOffset;
		break;
	case WARP_RIGHT:
		tmpPos.x += posOffset;
		break;
	case WARP_UP:
		tmpPos.z += posOffset;
		break;
	case WARP_DOWN:
		tmpPos.z -= posOffset;
		break;
	case WARP_DIR_END:
		break;
	default:
		break;
	}
	
	pWarp->Set_WarpPos(tmpPos);

	return pWarp;
}

void CWarp::Free()
{
	CGameObject::Free();
}
