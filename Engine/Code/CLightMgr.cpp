#include "CLightMgr.h"

IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr()
    : m_pGraphicDev(nullptr)
{
}

CLightMgr::~CLightMgr()
{
    Free();
}

HRESULT CLightMgr::Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
    CLight* pLight = CLight::Create(pGraphicDev, pLightInfo, iIndex);
    if (nullptr == pLight)
        return E_FAIL;

    m_LightList.push_back(pLight);

    return S_OK;
}

HRESULT CLightMgr::Ready_PointLight(LPDIRECT3DDEVICE9 pGraphicDev, const LIGHTDATA& lightData)
{
    if (nullptr == m_pGraphicDev)
    {
        m_pGraphicDev = pGraphicDev;
        m_pGraphicDev->AddRef();
    }

    // Point lights use indices starting from 1 (0 is reserved for directional light)
    _uint iIndex = static_cast<_uint>(m_vecPointLights.size()) + 1;

    CPointLight* pLight = CPointLight::Create(pGraphicDev, lightData, iIndex);
    if (nullptr == pLight)
        return E_FAIL;

    m_vecPointLights.push_back(pLight);

    return S_OK;
}

void CLightMgr::Update_PointLights(const _vec3& vPlayerPos)
{
    if (m_vecPointLights.empty())
        return;

    // Create distance-index pairs for sorting
    vector<pair<_float, _uint>> vecDistances;
    vecDistances.reserve(m_vecPointLights.size());

    for (_uint i = 0; i < m_vecPointLights.size(); ++i)
    {
        _float fDistSq = m_vecPointLights[i]->GetDistanceSq(vPlayerPos);
        vecDistances.push_back({ fDistSq, i });
    }

    // Sort by distance (ascending)
    sort(vecDistances.begin(), vecDistances.end(),
        [](const pair<_float, _uint>& a, const pair<_float, _uint>& b) {
            return a.first < b.first;
        });

    // Enable closest 8 lights (DX9 limit), disable rest
    const _uint MAX_ACTIVE_LIGHTS = 8;

    for (_uint i = 0; i < vecDistances.size(); ++i)
    {
        _uint idx = vecDistances[i].second;

        if (i < MAX_ACTIVE_LIGHTS)
            m_vecPointLights[idx]->Enable();
        else
            m_vecPointLights[idx]->Disable();
    }
}

void CLightMgr::Push_State()
{
    // 현재 상태를 스택에 보관 (삭제 안 함!)
    LIGHT_STATE tState;
    tState.LightList = m_LightList;
    tState.vecPointLights = m_vecPointLights;

    m_stackState.push(tState);

    // 새 씬용으로 비움
    m_LightList.clear();
    m_vecPointLights.clear();
}

void CLightMgr::Pop_State()
{
    if (m_stackState.empty())
        return;

    // 현재 상태 정리 (미니게임 라이트 등)
    for_each(m_vecPointLights.begin(), m_vecPointLights.end(), CDeleteObj());
    m_vecPointLights.clear();

    for_each(m_LightList.begin(), m_LightList.end(), CDeleteObj());
    m_LightList.clear();

    // 보관된 상태 복원
    LIGHT_STATE tState = m_stackState.top();
    m_stackState.pop();

    m_LightList = tState.LightList;
    m_vecPointLights = tState.vecPointLights;

    // 복원된 디렉셔널 라이트들 다시 활성화
    for (auto& pLight : m_LightList)
    {
        if (pLight)
            pLight->Enable();
    }

    // 포인트 라이트는 Update_PointLights()에서 자동 처리됨
}

void CLightMgr::Free()
{
    for_each(m_vecPointLights.begin(), m_vecPointLights.end(), CDeleteObj());
    m_vecPointLights.clear();

    // Clear directional lights
    for_each(m_LightList.begin(), m_LightList.end(), CDeleteObj());
    m_LightList.clear();

    Safe_Release(m_pGraphicDev);
}
