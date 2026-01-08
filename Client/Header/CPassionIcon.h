#pragma once
#include "CUi.h"
#include "Gage_Enum.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CPassionIcon :
	public CUi
{
private:
	explicit CPassionIcon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPassionIcon();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

	void						Set_GaugeState(Gauge::GAUGESTATE eState) { m_eCurState = eState; }

public:
	static CPassionIcon* Create(LPDIRECT3DDEVICE9 pGraphicDev, Gauge::GAUGESTATE eState);

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();
	void                        Update_State();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	Gauge::GAUGESTATE m_eCurState;
	Gauge::GAUGESTATE m_ePreState;
	_int m_iIndex;
};

