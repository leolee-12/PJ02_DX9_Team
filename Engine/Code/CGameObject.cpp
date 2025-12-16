#include "CGameObject.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev), m_pMessageChannel(nullptr), m_eOBJID(OID_END)
{
    m_pGraphicDev->AddRef();
}

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* SceneChannel)
    : m_pGraphicDev(pGraphicDev), m_pMessageChannel(SceneChannel), m_eOBJID(OID_END)
{
    m_pGraphicDev->AddRef();
    m_pMessageChannel->AddRef();
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_pGraphicDev(rhs.m_pGraphicDev), m_pMessageChannel(rhs.m_pMessageChannel), m_eOBJID(rhs.m_eOBJID)
{
    m_pGraphicDev->AddRef();
    if (m_pMessageChannel != nullptr) { m_pMessageChannel->AddRef(); }
}

CGameObject::~CGameObject()
{
}

CComponent* CGameObject::Get_Component(COMPONENTID eID, const _tchar* pComponentTag)
{
    CComponent* pComponent = Find_Component(eID, pComponentTag);

    if (nullptr == pComponent)
        return nullptr;

    return pComponent;
}

HRESULT CGameObject::Ready_GameObject()
{
    return S_OK;
}

_int CGameObject::Update_GameObject(const _float& fTimeDelta)
{
    if (m_iHp <= 0) {
        return DEAD;
    }
    for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
        pComponent.second->Update_Component(fTimeDelta);

    return NOEVENT;
}

void CGameObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
    for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
        pComponent.second->LateUpdate_Component();
}

void CGameObject::Render_GameObject()
{
}

CComponent* CGameObject::Find_Component(COMPONENTID eID, const _tchar* pComponentTag)
{
    auto        iter = find_if(m_mapComponent[eID].begin(),
                                m_mapComponent[eID].end(), 
                                 CTag_Finder(pComponentTag));

    if (iter == m_mapComponent[eID].end())
        return nullptr;

    return iter->second;
}

void CGameObject::Free()
{
    for (_uint i = 0; i < ID_END; ++i)
    {
        for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
        m_mapComponent[i].clear();
    }
    Unsubscribe_Handles();
    Safe_Release(m_pGraphicDev);
    Safe_Release(m_pMessageChannel);
}

void CGameObject::Unsubscribe_Handles()
{
    for (auto iter = m_hmapSubHandles.begin();
        iter != m_hmapSubHandles.end();) 
    {
        m_pMessageChannel->Unsubscribe(iter->second);
        iter = m_hmapSubHandles.erase(iter);
    }
}


