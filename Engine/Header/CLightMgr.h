#pragma once
#include "CBase.h"
#include "CLight.h"
#include "CPointLight.h"
#include <vector>
#include <algorithm>

BEGIN(Engine)

class ENGINE_DLL CLightMgr : public CBase
{
	DECLARE_SINGLETON(CLightMgr)

private:
	explicit CLightMgr();
	virtual ~CLightMgr();

public:
	// Existing directional light
	HRESULT Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,
		const D3DLIGHT9* pLightInfo,
		const _uint& iIndex);

	// Point light functions
	HRESULT Ready_PointLight(LPDIRECT3DDEVICE9 pGraphicDev,
		const LIGHTDATA& lightData);

	void Update_PointLights(const _vec3& vPlayerPos);

private:
	list<CLight*>				m_LightList;
	vector<CPointLight*>		m_vecPointLights;
	LPDIRECT3DDEVICE9			m_pGraphicDev;

private:
	virtual void		Free();
};

END
