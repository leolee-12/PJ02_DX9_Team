#pragma once
#include "CUi.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

enum BUILDING_TYPE;

class CBuildingSelectSlot : public CUi
{
private:
	explicit	CBuildingSelectSlot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		~CBuildingSelectSlot();

public:
	void            Set_Hovered(_bool b) { m_bHovered = b; }
	void            Set_CanBuild(_bool b) { m_bCanBuild = b; }
	BUILDING_TYPE   Get_BuildingType() const { return m_eBuildingType; }
	_bool           Get_CanBuild() const { return m_bCanBuild; }

	virtual	HRESULT		Ready_GameObject();
	virtual	_int		Update_GameObject(const _float& fTimeDelta);
	virtual	void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void		Render_GameObject();

	_bool				Is_Clicked();
	_bool				Is_Hovered();

private:
	HRESULT Add_Component();

private:
	CRcTex*			m_pBufferCom;
	CTransform*		m_pTransformCom;
	CTexture*		m_pSlotTextureCom;	// 슬롯 프레임
	CTexture*		m_pIconTextureCom;	// 건물 아이콘

	// 슬롯 정보
	BUILDING_TYPE   m_eBuildingType;
	_bool           m_bHovered;
	_bool           m_bCanBuild;		// 자원 충분 여부

	_float          m_fWidth;
	_float          m_fHeight;

public:
	static CBuildingSelectSlot* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, BUILDING_TYPE eType);

private:
	virtual void Free() override;
};
