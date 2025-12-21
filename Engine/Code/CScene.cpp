#include "CScene.h"

CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev), m_pMessageChannel(nullptr)
{
    m_pGraphicDev->AddRef();
}

CScene::~CScene()
{
}

CComponent* CScene::Get_Component(COMPONENTID eID, const wstring& strLayerTag, const wstring& strObjTag, const wstring& strComponentTag)
{
    auto    iter = m_mapLayer.find(strLayerTag);

    if (iter == m_mapLayer.end())
        return nullptr;

    return iter->second->Get_Component(eID, strObjTag, strComponentTag);
}

HRESULT CScene::Ready_Scene()
{
    return S_OK;
}

_int CScene::Update_Scene(const _float& fTimeDelta)
{
    for (auto& pLayer : m_mapLayer)
        pLayer.second->Update_Layer(fTimeDelta);

    return NOEVENT;
}

void CScene::LateUpdate_Scene(const _float& fTimeDelta)
{
    for (auto& pLayer : m_mapLayer)
        pLayer.second->LateUpdate_Layer(fTimeDelta);
}



void CScene::Free()
{
    for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
    m_mapLayer.clear();

    Safe_Release(m_pGraphicDev);
    Safe_Release(m_pMessageChannel);
}
