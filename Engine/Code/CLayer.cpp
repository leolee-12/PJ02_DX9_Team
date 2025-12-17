#include "CLayer.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

CComponent* CLayer::Get_Component(COMPONENTID eID, const _tchar* pObjTag, const _tchar* pComponentTag)
{
	auto iter = m_mapObject.find(pObjTag);
	if (iter == m_mapObject.end()) { return nullptr; }

	return iter->second.front()->Get_Component(eID, pComponentTag);
}

HRESULT CLayer::Add_GameObject(const _tchar* pObjTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	auto& vec = m_mapObject[pObjTag];

	vec.push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

_int CLayer::Update_Layer(const _float& fTimeDelta)
{
	_int	iResult(0);

	/*for (auto& pObj : m_mapObject)
	{
		iResult = pObj.second->Update_GameObject(fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}*/

	for (auto iter = m_mapObject.begin();
		iter != m_mapObject.end();
		++iter)
	{
		auto& vecOrigin = iter->second;

		for (auto veciter = vecOrigin.begin();
			veciter != vecOrigin.end();)
		{
			iResult = (*veciter)->Update_GameObject(fTimeDelta);
			if (iResult == DEAD) {
				Safe_Release((*veciter));
				veciter = vecOrigin.erase(veciter);
			}
			else {
				++veciter;
			}
		}
	}

	return NOEVENT;
}

void CLayer::LateUpdate_Layer(const _float& fTimeDelta)
{
	for (auto iter = m_mapObject.begin();
		iter != m_mapObject.end();
		++iter)
	{
		for (auto& vec : iter->second) {
			vec->LateUpdate_GameObject(fTimeDelta);
		}
	}
}




CLayer* CLayer::Create()
{
	CLayer* pLayer = new CLayer();

	if (FAILED(pLayer->Ready_Layer()))
	{
		MSG_BOX("Layer Create Failed");
		Safe_Release(pLayer);
		return nullptr;
	}

	return pLayer;
}

void CLayer::Free()
{
	for (auto iter = m_mapObject.begin();
		iter != m_mapObject.end();
		++iter)
	{
		auto& vecOrigin = iter->second;
		for (auto veciter = vecOrigin.begin();
			veciter != vecOrigin.end();) 
		{
			Safe_Release((*veciter));
			veciter = vecOrigin.erase(veciter);
		}
	}
}
