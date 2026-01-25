#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

enum FOODTYPE
{
	FT_NOMAL,
	FT_COUNT,
	FT_QUALITY,
	FT_END
};

class CCookingTargetFood :
	public CUi
{
private:
	explicit CCookingTargetFood(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCookingTargetFood();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CCookingTargetFood* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fScale, FOODTYPE ft, _int iQulity = 0);

	void Set_Render(_bool _bRender) { m_bRender = _bRender; };
	_int Get_CraftableCount() { return m_iCraftableCount; }
	_int Set_CraftableCount(_int _iCount) { return m_iCraftableCount = _iCount; }
	void ApplayCraftableCount(_int _iCount) { m_iCraftableCount += _iCount; }

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	vector<CGameObject*> m_vecQualityStar;

	_vec3 m_vPos;
	_float m_fScale;
	FOODTYPE m_eFoodType;
	_int	m_iQualityStar;
	_int	m_iCraftableCount;

	_bool m_bRender;
};

