#pragma once
#include "CUi.h"
#include "Building_Enum.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class	CBuildingSelectSlot;
class	CBuildingInfoCard;

class CBuildingCraftCtrl : public CUi
{
public:
	enum BUILDING_CRAFT_STATE { BCS_IDLE, BCS_SELECT, BCS_BUILD, BCS_END };
	enum TEXTURE_TYPE { BACKGROUND, DECO1, DECO2, TT_END };

private:
	explicit	CBuildingCraftCtrl(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		~CBuildingCraftCtrl();

public:
	virtual	HRESULT		Ready_GameObject();
	virtual	_int		Update_GameObject(const _float& fTimeDelta);
	virtual	void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void		Render_GameObject();

	void				Open();    // BCS_SELECT 진입
	void				Close();   // BCS_IDLE 복귀

private:
	HRESULT				Add_Component();
	HRESULT				Ready_Slots();		// 슬롯 생성
	HRESULT				Ready_InfoCard();	// InfoCard 생성
	void				Ready_Event();		// 이벤트 구독

	void				Update_Select(const _float& fTimeDelta);
	void				Update_Build(const _float& fTimeDelta);
	void				Update_SlotHover();	// 호버 감지 -> 인포카드 갱신
	void				On_SlotClicked(BUILDING_TYPE eType);
	_vec3				ScreenToDX(const _float& fX, const _float& fY, const _float& fZ = 0.f);
	void				DXToScreen(const _vec3& vDX, _float& fScreenX, _float& fScreenY);

private:
	// 컴포넌트
	CRcTex*		m_pBufferCom;
	CTransform* m_pTransformCom[TT_END];
	CTexture*	m_pTextureCom[TT_END];      // 카드 배경

	// 하위 UI
	vector<CBuildingSelectSlot*>    m_vecSlots;
	CBuildingInfoCard*				m_pInfoCard;

	// 호버 추적
	CBuildingSelectSlot* m_pHoveredSlot;

	BUILDING_CRAFT_STATE m_eState;
	BUILDING_CRAFT_STATE m_ePreState = BCS_END;  // 초기화 필수!

public:
	static CBuildingCraftCtrl* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);

private:
	virtual void Free() override;
};
