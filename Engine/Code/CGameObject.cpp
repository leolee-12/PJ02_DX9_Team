#include "CGameObject.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev), m_pMessageChannel(nullptr), m_eOBJID(OID_END)
    , m_fDepth(0.f)
{
    m_pGraphicDev->AddRef();
}

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* SceneChannel)
    : m_pGraphicDev(pGraphicDev), m_pMessageChannel(SceneChannel), m_eOBJID(OID_END)
    , m_fDepth(0.f)
{
    m_pGraphicDev->AddRef();
    m_pMessageChannel->AddRef();
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_pGraphicDev(rhs.m_pGraphicDev), m_pMessageChannel(nullptr), m_eOBJID(rhs.m_eOBJID)
    , m_fDepth(rhs.m_fDepth), m_vDepth(rhs.m_vDepth)
{
	m_pGraphicDev->AddRef();
}

CGameObject::~CGameObject()
{
}

CComponent* CGameObject::Get_Component(COMPONENTID eID, const wstring& strComponentTag)
{
    CComponent* pComponent = Find_Component(eID, strComponentTag);

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

void CGameObject::OnCollision(CGameObject* pObject)
{
}

void CGameObject::Compute_ViewDepth(const _vec3* pPos)
{
    _matrix     matCamWorld;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
    D3DXMatrixInverse(&matCamWorld, 0, &matCamWorld);

    _vec3   vCamPos;
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

    _vec3      vDir = vCamPos - *pPos;

	m_vDepth = *pPos;
}

CComponent* CGameObject::Find_Component(COMPONENTID eID, const wstring& strComponentTag)
{
    auto iter = m_mapComponent[eID].find(strComponentTag);

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

void CGameObject::Requst_Unsubscribe(const wstring& strEventType)
{
    auto iter = m_hmapSubHandles.find(strEventType);
    if (iter == m_hmapSubHandles.end()) { return; }

    m_pMessageChannel->Unsubscribe(iter->second);
    m_hmapSubHandles.erase(iter);
}


