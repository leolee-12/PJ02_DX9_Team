#include "CPointLight.h"

USING(Engine)

// Intensity presets: Strong, Medium, Weak
const _float CPointLight::PRESET_RANGE[3] = { 30.f, 20.f, 10.f };
const _float CPointLight::PRESET_ATTENUATION[3] = { 0.05f, 0.1f, 0.2f };

CPointLight::CPointLight(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_iIndex(0)
	, m_vPosition(0.f, 0.f, 0.f)
	, m_iIntensity(1)
	, m_bEnabled(false)
{
	ZeroMemory(&m_tLight, sizeof(D3DLIGHT9));
	m_pGraphicDev->AddRef();
}

CPointLight::~CPointLight()
{
}

HRESULT CPointLight::Ready_PointLight(const LIGHTDATA& lightData, const _uint& iIndex)
{
	m_iIndex = iIndex;
	m_vPosition = { lightData.x, lightData.y, lightData.z };
	m_iIntensity = lightData.intensity;

	// Clamp intensity to valid range
	if (m_iIntensity < 0) m_iIntensity = 0;
	if (m_iIntensity > 2) m_iIntensity = 2;

	// Setup D3DLIGHT9 for Point Light
	ZeroMemory(&m_tLight, sizeof(D3DLIGHT9));

	m_tLight.Type = D3DLIGHT_POINT;
	
	// light color
	m_tLight.Diffuse = D3DXCOLOR(0.8f, 0.3f, 0.3f, 1.0f);
	m_tLight.Ambient = D3DXCOLOR(0.f, 0.05f, 0.05f, 1.0f);
	//m_tLight.Specular = D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f);
	 
	//m_tLight.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.0f);
	m_tLight.Specular = D3DXCOLOR(1.f, 0.f, 0.f, 1.0f);

	// Position
	m_tLight.Position = D3DXVECTOR3(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// Attenuation based on intensity preset
	//m_tLight.Range = PRESET_RANGE[m_iIntensity];
	m_tLight.Range = 80;
	m_tLight.Attenuation0 = 1.f;
	//m_tLight.Attenuation1 = PRESET_ATTENUATION[m_iIntensity];
	m_tLight.Attenuation1 = 0.0f;
	m_tLight.Attenuation2 = 0.012f;

	// Set light but don't enable yet
	m_pGraphicDev->SetLight(iIndex, &m_tLight);
	m_bEnabled = false;

	return S_OK;
}

void CPointLight::Enable()
{
	if (!m_bEnabled)
	{
		m_pGraphicDev->LightEnable(m_iIndex, TRUE);
		m_bEnabled = true;
	}
}

void CPointLight::Disable()
{
	if (m_bEnabled)
	{
		m_pGraphicDev->LightEnable(m_iIndex, FALSE);
		m_bEnabled = false;
	}
}

_float CPointLight::GetDistanceSq(const _vec3& vPos) const
{
	_float dx = m_vPosition.x - vPos.x;
	_float dy = m_vPosition.y - vPos.y;
	_float dz = m_vPosition.z - vPos.z;

	return dx * dx + dy * dy + dz * dz;
}

CPointLight* CPointLight::Create(LPDIRECT3DDEVICE9 pGraphicDev, const LIGHTDATA& lightData, const _uint& iIndex)
{
	CPointLight* pLight = new CPointLight(pGraphicDev);

	if (FAILED(pLight->Ready_PointLight(lightData, iIndex)))
	{
		Safe_Release(pLight);
		return nullptr;
	}

	return pLight;
}

void CPointLight::Free()
{
	if (m_bEnabled)
	{
		m_pGraphicDev->LightEnable(m_iIndex, FALSE);
	}

	Safe_Release(m_pGraphicDev);
}
