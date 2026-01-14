#include "CProtoMgr.h"

IMPLEMENT_SINGLETON(CProtoMgr)

CProtoMgr::CProtoMgr()
{
}

CProtoMgr::~CProtoMgr()
{
    Free();
}

HRESULT CProtoMgr::Ready_Prototype(const wstring& pComponentTag, CComponent* pComponent)
{
    CComponent* pInstance = Find_Prototype(pComponentTag);

    if (nullptr != pInstance) {
        Safe_Release(pComponent);
        return E_FAIL;
    }

    m_mapPrototype.insert({ pComponentTag ,pComponent });

    return S_OK;
}

CComponent* CProtoMgr::Clone_Prototype(const wstring& pComponentTag)
{
    CComponent* pComponent = Find_Prototype(pComponentTag);

    if (nullptr == pComponent)
        return nullptr;

    return pComponent->Clone();
}

CComponent* CProtoMgr::Find_Prototype(const wstring& pComponentTag)
{
    auto iter = m_mapPrototype.find(pComponentTag);

    if (iter == m_mapPrototype.end())
        return nullptr;

    return iter->second;
}

void CProtoMgr::Free()
{
    for_each(m_mapPrototype.begin(), m_mapPrototype.end(), CDeleteMap());
    m_mapPrototype.clear();
}
