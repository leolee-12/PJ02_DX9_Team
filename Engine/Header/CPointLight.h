#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "Engine_Struct.h"

BEGIN(Engine)

// CLight와 별개로 CBase를 직접 상속 (점 조명과 방향성 조명은 다른 개념)
class ENGINE_DLL CPointLight : public CBase
{
private:
	explicit CPointLight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPointLight();

public:
	HRESULT		Ready_PointLight(const LIGHTDATA& lightData, const _uint& iIndex);

	void		Enable();
	void		Disable();
	_bool		IsEnabled() const { return m_bEnabled; }

	_vec3		GetPosition() const { return m_vPosition; }
	_float		GetDistanceSq(const _vec3& vPos) const;

	_uint		GetIndex() const { return m_iIndex; }

public:
	// Intensity presets
	static const _float PRESET_RANGE[3];
	static const _float PRESET_ATTENUATION[3];

private:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_uint				m_iIndex;
	_vec3				m_vPosition;
	_int				m_iIntensity;
	_bool				m_bEnabled;
	D3DLIGHT9			m_tLight;

public:
	static CPointLight* Create(LPDIRECT3DDEVICE9 pGraphicDev,
							   const LIGHTDATA& lightData,
							   const _uint& iIndex);

private:
	virtual void	Free();
};

END
