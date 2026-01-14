#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CCookingGauge :
	public CUi
{
private:
	explicit CCookingGauge(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCookingGauge();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CCookingGauge* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	void Set_RandomPosX();
	void Get_GaugeRange(float& fLeft, float& fRight);

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	float fxRange;
};

