#include "pch.h"
#include "CMapWarp.h"
#include "CCollider.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CWarp.h"

CMapWarp::CMapWarp(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{

}

CMapWarp::CMapWarp(const CMapWarp& rhs)
	: CGameObject(rhs)
{

}

CMapWarp::~CMapWarp()
{
}

HRESULT CMapWarp::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CMapWarp::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (Warps.first)  Warps.first->Update_GameObject(fTimeDelta);
	if (Warps.second) Warps.second->Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_FLOOR, this);

	return iExit;
}

void CMapWarp::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMapWarp::Render_GameObject()
{
	if (!bRender) return;

	// 일단은 그냥 사각형 찍기 (바닥에)
	// collider comp에 맞춰서 찍어줘야 함
	Warps.first->Render_GameObject();
	Warps.second->Render_GameObject();
}

void CMapWarp::OnCollision(CGameObject* pObject)
{

}

void CMapWarp::Set_Warps(CWarp* Warp1, CWarp* Warp2)
{
	Warps.first = Warp1;
	Warps.second = Warp2;
}

CWarp* CMapWarp::Get_OtherWarp(CWarp* curWarp)
{
	if (Warps.first == curWarp)
	{
		return Warps.second;
	}
	else if (Warps.second == curWarp)
	{
		return Warps.first;
	}
	else
	{
		return nullptr;
	}
}

HRESULT CMapWarp::Add_Component()
{
	return S_OK;
}

CMapWarp* CMapWarp::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, _vec3 Warp1Pos, _uint Warp1Dir, _vec3 Warp2Pos, _uint Warp2Dir)
{
	CMapWarp* pMapWarp = new CMapWarp(pGraphicDev);

	if (FAILED(pMapWarp->Ready_GameObject()))
	{
		Safe_Release(pMapWarp);
		MSG_BOX("CMapWarp Create Failed");
		return nullptr;
	}
	
	CWarp* pWarp1 = CWarp::Create(pGraphicDev, pMessageChannel, pMapWarp, Warp1Pos, (WarpDir)Warp1Dir);
	CWarp* pWarp2 = CWarp::Create(pGraphicDev, pMessageChannel, pMapWarp, Warp2Pos, (WarpDir)Warp2Dir);

	pMapWarp->Set_Warps(pWarp1, pWarp2);

	return pMapWarp;
}

void CMapWarp::Free()
{
	Safe_Release(Warps.first);
	Safe_Release(Warps.second);
	CGameObject::Free();
}
