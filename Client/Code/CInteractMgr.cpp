#include "pch.h"
#include "CInteractMgr.h"
#include "CGameObject.h"
#include "CTransform.h"
#include <CBreakableTree.h>
#include <CBreakableRock.h>

IMPLEMENT_SINGLETON(CInteractMgr);

CInteractMgr::CInteractMgr()
{
}

CInteractMgr::~CInteractMgr()
{
	Free();
}

HRESULT CInteractMgr::Ready_InteractMgr()
{
	m_mapInteractables.emplace(WOOD,	list<CGameObject*>());
	m_mapInteractables.emplace(ROCK,	list<CGameObject*>());
	m_mapInteractables.emplace(BUILD,	list<CGameObject*>());
	m_mapInteractables.emplace(FOOD,	list<CGameObject*>());
	m_mapInteractables.emplace(PRAY,	list<CGameObject*>());

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

	iter->second.push_back(pIObj);	// 리스트에 추가
	pIObj->AddRef();				// 참조카운트 Up
}

void CInteractMgr::Unregister_IObj(INTERACT_TYPE eType, CGameObject* pIObj)
{
	auto iter = m_mapInteractables.find(eType);

	if (iter == m_mapInteractables.end()) return;

	auto& pIObjList = iter->second;
	auto TargetIter = find(pIObjList.begin(), pIObjList.end(), pIObj);

	if (TargetIter != pIObjList.end())
	{
		pIObjList.erase(TargetIter);	// 리스트에서 제거
		Safe_Release(pIObj);			// 참조카운트 Down
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
		vTargetDir.y = 0.f;
		_float fDistance = D3DXVec3Length(&vTargetDir);

		if (fDistance < fMinDistance)
		{
			fMinDistance = fDistance;
			pNearest = pIObj;
		}
	}

	return pNearest;
}

_bool CInteractMgr::Apply_Work(INTERACT_TYPE eType, const _vec3& vPos, const float& fWork)
{
	CGameObject* pTarget = Find_Nearest(eType, vPos);

	if (!pTarget) return false;

	IInteractable* pIObj = dynamic_cast<IInteractable*>(pTarget);

	if (pIObj)
	{
		pIObj->Add_WorkGauge(fWork);

		if (pIObj->Is_WorkComplete())	return false;
		else							return true;
	}
	else return false;
}

void CInteractMgr::Clear_IObj()
{
	for (auto& pair : m_mapInteractables)
	{
		for (auto& pIObj : pair.second)
		{
			Safe_Release(pIObj);	// Unregister는 리스트에서 제거해버리므로 iter 무효화 -> Safe_Release()만
		}
		pair.second.clear();
	}
	
	m_mapInteractables.clear();
}

void CInteractMgr::Free()
{
	Clear_IObj();
}
