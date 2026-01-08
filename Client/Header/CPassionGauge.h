#pragma once
#include "CUi.h"
#include "Gage_Enum.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}


class CPassionGauge :
	public CUi
{
private:
	explicit CPassionGauge(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPassionGauge();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

	void						Set_GaugeState(Gauge::GAUGESTATE eState) { m_eCurState = eState; }
	void                        Set_GaugeValue(_float fValue) { m_fGaugeValue = fValue; }

	void                        Set_PassionMax(_float fPassionMax) { m_fMaxPassionGaugeValue = fPassionMax; }
	void                        Set_FaithMax(_float fFaithMax) { m_fMaxFaithGaugeValue = fFaithMax; }

public:
	static CPassionGauge* Create(LPDIRECT3DDEVICE9 pGraphicDev, Gauge::GAUGESTATE eState, _float fPassionMax, _float fFaithMax);

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();
	HRESULT 					Ready_PixelShader();
	void                        Update_State();
	void                        Update_ClampGageValue();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;



	Gauge::GAUGESTATE m_eCurState;
	Gauge::GAUGESTATE m_ePreState;
	D3DXCOLOR		  m_tColor;

	_float			  m_fGaugeValue;
	_float            m_fMaxPassionGaugeValue;
	_float            m_fMaxFaithGaugeValue;

	_float            m_fClampGaugeValue;

	LPDIRECT3DPIXELSHADER9 m_pPixelShader;
};

