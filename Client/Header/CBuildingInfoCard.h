#pragma once
#include "CUi.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CFontUIOrtho;
class CBuildingSlot;
enum BUILDING_TYPE;

class CBuildingInfoCard : public CUi
{
public:
	enum TEXTURE_TYPE { BACKGROUND, ICON, INGREDIENT1, INGREDIENT2, TT_END };

private:
	explicit	CBuildingInfoCard(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		~CBuildingInfoCard();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual	_int		Update_GameObject(const _float& fTimeDelta);
	virtual	void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void		Render_GameObject();

	void				Show(BUILDING_TYPE eType, const _vec3& vSlotPos);
	void				Hide();

private:
	HRESULT				Add_Component();
	void				Update_Content(BUILDING_TYPE eType);  // 텍스트 갱신

	// 컴포넌트
	CRcTex*			m_pBufferCom;
	CTransform*		m_pTransformCom[TT_END];
	CTexture*		m_pTextureCom[TT_END];

	// 폰트 UI
	CFontUIOrtho*	m_pNameFont;        // 건물 이름
	CFontUIOrtho*	m_pCostFont;        // 필요 자원

	// 상태
	_bool           m_bVisible;
	BUILDING_TYPE   m_eDisplayType;     // 현재 표시 중인 건물

public:
	static CBuildingInfoCard* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};
