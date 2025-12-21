#include "pch.h"
#include "CPersistentMgr.h"
#include "CPlayer.h"

CPersistentMgr* CPersistentMgr::m_pInstance = nullptr;

CPersistentMgr::CPersistentMgr()
{
}

CPersistentMgr::~CPersistentMgr()
{
    Free();
}

HRESULT CPersistentMgr::Ready_GlobalObjects(LPDIRECT3DDEVICE9 pGraphicDev)
{
    if (m_hmapGlobalObjects.empty())
    {
        CGameObject* pGameObject = CPlayer::Create(pGraphicDev);

        if (nullptr == pGameObject)
            return E_FAIL;

        m_hmapGlobalObjects[GOBJ_PLAYER] = pGameObject;
    }
    return S_OK;
}

CGameObject* CPersistentMgr::Get_GlobalObjects(GOBJID eGOBJID)
{
    auto iter = m_hmapGlobalObjects.find(eGOBJID);
    if (iter == m_hmapGlobalObjects.end()) { return nullptr; }

    return iter->second;
}

void CPersistentMgr::Free()
{
    for (auto& hmap : m_hmapGlobalObjects)
    {
        m_vecReleaseQueue.push_back(hmap.second);
        hmap.second = nullptr;
    }

    for (auto& Obj : m_vecReleaseQueue)
    {
        Safe_Release(Obj);
    }
    m_vecReleaseQueue.clear();
}
