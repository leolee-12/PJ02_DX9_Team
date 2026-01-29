#pragma once
#include "CUi.h"
#include "Building_Enum.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CFontUIOrtho;

class CBuildingInfoCard : public CUi
{
public:
	enum TEXTURE_TYPE { BACKGROUND, ICONBACK, ICON, INGREDIENT1, INGREDIENT2, TT_END };
	enum FONT_ID { FID_NAME, FID_DATA, FID_COST1, FID_RESERVE1, FID_COST2, FID_RESERVE2, FID_END };

	typedef struct tagFontInfo
	{
		_vec2		vPos;
		_vec2		vScale;
		D3DXCOLOR	tColor;
		_uint		uFlags;
		wstring		strText;
		wstring		strFontKey;
	}FONTINFO;

private:
	explicit	CBuildingInfoCard(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		~CBuildingInfoCard();

public:
	void				Set_Pos(TEXTURE_TYPE eType, const _vec3& vPos);
	void				Set_Scale(TEXTURE_TYPE eType, const _vec3& vScale);
	void				Set_Reserve(const _uint& iReserve1, const _uint& iReserve2);
	void				Set_FontPos(FONT_ID eID, const _float& fX, const _float& fY);

	virtual	HRESULT		Ready_GameObject();
	virtual	_int		Update_GameObject(const _float& fTimeDelta);
	virtual	void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void		Render_GameObject();

	void				Show(BUILDING_TYPE eType);
	void				Hide();

private:
	HRESULT				Add_Component();
	HRESULT				Ready_MyFont();
	void				Update_Content(BUILDING_TYPE eType);  // 텍스트 갱신

	// 컴포넌트
	CRcTex*			m_pBufferCom;
	CTransform*		m_pTransformCom[TT_END];
	CTexture*		m_pTextureCom[TT_END];

	// 폰트 UI
	CFontUIOrtho*	m_pFont;
	FONTINFO		m_tFontInfo[FID_END];

	// 상태
	_bool           m_bVisible;
	BUILDING_TYPE   m_eDisplayType;     // 현재 표시 중인 건물
	_uint			m_iTexIdx[TT_END];

public:
	static CBuildingInfoCard* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free() override;
};
