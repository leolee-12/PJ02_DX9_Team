#include "pch.h"
#include "CInteractMgr.h"
#include "CGameObject.h"
#include "CTransform.h"

IMPLEMENT_SINGLETON(CInteractMgr);

CInteractMgr::CInteractMgr()
{
}

CInteractMgr::~CInteractMgr()
{
}

HRESULT CInteractMgr::Ready_InteractMgr()
{
	m_mapInteractables.emplace(WOOD, list<CGameObject*>());
	m_mapInteractables.emplace(ROCK, list<CGameObject*>());
	m_mapInteractables.emplace(BUILD, list<CGameObject*>());
	m_mapInteractables.emplace(FOOD, list<CGameObject*>());
	m_mapInteractables.emplace(PRAY, list<CGameObject*>());

	return S_OK;
}

void CInteractMgr::Register_IObj(INTERACT_TYPE eType, CGameObject* pIObj)
{
	auto iter = m_mapInteractables.find(eType);

	if (iter == m_mapInteractables.end()) return;

	for (auto pRegisteredObj : iter->second)
	{
		if (pRegisteredObj == pIObj) return;
	}

	iter->second.push_back(pIObj);
	pIObj->AddRef();
}

void CInteractMgr::Unregister_IObj(INTERACT_TYPE eType, CGameObject* pIObj)
{
	auto iter = m_mapInteractables.find(eType);

	if (iter == m_mapInteractables.end()) return;

	for (auto pRegisteredObj : iter->second)
	{
		if (pRegisteredObj == pIObj)
		{
			Safe_Release(pIObj);
			return;
		}
	}
}

CGameObject* CInteractMgr::Find_Nearest(INTERACT_TYPE eType, const _vec3& vPos)
{
	auto iter = m_mapInteractables.find(eType);

	if (iter == m_mapInteractables.end()) return nullptr;

	_float fMinDistance = FLT_MAX;

	CGameObject* pNearest = nullptr;

	for (auto& pIObj : iter->second)
	{
		_vec3 vIObjPos{};
		static_cast<CTransform*>(pIObj->Get_Component(ID_DYNAMIC, L"Com_Transform"))->Get_Info(INFO_POS, &vIObjPos);

		_vec3 vTargetDir = vIObjPos - vPos;
		_float fDistance = D3DXVec3Length(&vTargetDir);

		if (fDistance < fMinDistance)
		{
			fMinDistance = fDistance;
			pNearest = pIObj;
		}
	}

	return pNearest;
}

void CInteractMgr::Clear_IObj()
{
	for (auto& pair : m_mapInteractables)
	{
		for (auto& pIObj : pair.second)
		{
			Unregister_IObj(pair.first, pIObj);
		}
	}

	m_mapInteractables.clear();
}

void CInteractMgr::Free()
{
	Clear_IObj();
}
