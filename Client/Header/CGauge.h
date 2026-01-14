#pragma once
#include "CUi.h"
#include "Gage_Enum.h"


namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CPassionGauge;
class CPassionIcon;
class CGaugeStar;


class CGauge :
	public CUi
{
private:
	explicit CGauge(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGauge();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

	void                        Set_GaugeState(Gauge::GAUGESTATE eState) { m_eCurGaugeState = eState; }
	void  						Set_GaugeValue(_float fValue) { m_fGaugeValue = fValue; }
	void  						Add_GaugeValue(_float fValue) { m_fGaugeValue += fValue; }

	void                        Set_PassionMax(_float fPassionMax) { m_fMaxPassionGaugeValue = fPassionMax; }
	void                        Set_FaithMax(_float fFaithMax) { m_fMaxFaithGaugeValue = fFaithMax; }

public:
	static CGauge* Create(LPDIRECT3DDEVICE9 pGraphicDev, Gauge::GAUGESTATE eState);

private:
	virtual			void		Free();

	void                        Update_State();

	void                        Accumulate_GaugeValue(const _float& fTimeDelta);

private:
	Gauge::GAUGESTATE	 m_eCurGaugeState;
	Gauge::GAUGESTATE	 m_ePreGaugeState;
	vector<CGameObject*> m_vecGaugeUI;
	
	CPassionGauge*		 m_pPassionGauge;
	CPassionIcon*		 m_pPassionIcon;
	CGaugeStar*			 m_pGaugeStar;

	_float               m_fGaugeValue;
	_bool                m_bPontRender;

	_float				 m_fMaxPassionGaugeValue;
	_float				 m_fMaxFaithGaugeValue;
};

