#include "pch.h"
#include "CBuildingCraftCtrl.h"
#include "CBuildingInfoCard.h"
#include "CBuildingSelectSlot.h"
#include "CFontUIOrtho.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CBuildingCraftCtrl::CBuildingCraftCtrl(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pInfoCard(nullptr)
	, m_pHoveredSlot(nullptr)
	, m_eState(BCS_IDLE)
	, m_ePreState(BCS_END)
{
	m_vecSlots.reserve(4);
}

CBuildingCraftCtrl::~CBuildingCraftCtrl()
{
}

HRESULT	CBuildingCraftCtrl::Ready_GameObject()
{
	if(FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Ready_MyTransform()))
		return E_FAIL;

	if (FAILED(Ready_Slots()))
		return E_FAIL;

	if (FAILED(Ready_InfoCard()))
		return E_FAIL;

	if (FAILED(Ready_TitleFont()))
		return E_FAIL;

	Ready_Event();

	m_eState = BCS_IDLE;
	m_pHoveredSlot = nullptr;

	return S_OK;
}

_int CBuildingCraftCtrl::Update_GameObject(const _float& fTimeDelta)
{
	if (BCS_IDLE == m_eState) return NOEVENT;

	switch (m_eState)
	{
	case BCS_SELECT:
		Update_Select(fTimeDelta);
		break;
	case BCS_BUILD:
		Update_Build(fTimeDelta);
		break;
	}

	return NOEVENT;
}

void CBuildingCraftCtrl::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CBuildingCraftCtrl::Render_GameObject()
{
	if (BCS_SELECT != m_eState) return;

	// 이상 없으면 반복문으로 변경
	// 배경 렌더
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[BACKGROUND]->Get_World());
	m_pTextureCom[BACKGROUND]->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	// 데코1 렌더
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[DECO1]->Get_World());
	m_pTextureCom[DECO1]->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	// 데코2 렌더
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[DECO2]->Get_World());
	m_pTextureCom[DECO2]->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	// 슬롯 렌더
	for (auto& pSlot : m_vecSlots)
		pSlot->Render_GameObject();

	// 인포카드 렌더
	if (m_pInfoCard)
		m_pInfoCard->Render_GameObject();

	// 타이틀 폰트
	if (m_pTitleFont)
		m_pTitleFont->Render_GameObject();
}

void CBuildingCraftCtrl::Open()
{
	m_eState = BCS_SELECT;
	m_pTitleFont->Active();
}

void CBuildingCraftCtrl::Close()
{
	m_eState = BCS_IDLE;
	m_pHoveredSlot = nullptr;
	m_pTitleFont->UnActive();

	for (auto& pSlot : m_vecSlots)
		pSlot->Set_Hovered(false);

	m_pInfoCard->Hide();
}

HRESULT	CBuildingCraftCtrl::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom[BACKGROUND] = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform_BG", pComponent });

	pComponent = m_pTransformCom[DECO1] = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform_Deco1", pComponent });

	pComponent = m_pTransformCom[DECO2] = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform_Deco2", pComponent });

	// Texture
	pComponent = m_pTextureCom[BACKGROUND] = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingSelectBack"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BG", pComponent });

	pComponent = m_pTextureCom[DECO1] = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingCardInfoLeftPattern"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Deco1", pComponent });

	pComponent = m_pTextureCom[DECO2] = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingCardInfoRightPattern"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Deco2", pComponent });

	return S_OK;
}

HRESULT CBuildingCraftCtrl::Ready_MyTransform()
{
	_vec3 vBackPos = ScreenToDX(WINCX * 0.25f, WINCY * 0.5f, 0.5f);
	m_pTransformCom[BACKGROUND]->Set_Pos(vBackPos.x, vBackPos.y, vBackPos.z);
	m_pTransformCom[BACKGROUND]->Set_Scale(WINCX * 0.5f, WINCY, 1.f);

	_float fDecoScale = 0.5f;

	_vec3 vDeco1Pos = ScreenToDX(240.f, 80.f, 0.4f);
	m_pTransformCom[DECO1]->Set_Pos(vDeco1Pos.x, vDeco1Pos.y, vDeco1Pos.z);
	m_pTransformCom[DECO1]->Set_Scale(116.f * fDecoScale, 51.f * fDecoScale, 1.f);

	_vec3 vDeco2Pos = ScreenToDX(400.f, 80.f, 0.4f);
	m_pTransformCom[DECO2]->Set_Pos(vDeco2Pos.x, vDeco2Pos.y, vDeco2Pos.z);
	m_pTransformCom[DECO2]->Set_Scale(118.f * fDecoScale, 51.f * fDecoScale, 1.f);

	return S_OK;
}

HRESULT CBuildingCraftCtrl::Ready_Slots()
{
	BUILDING_TYPE arrTypes[4] = { BT_COOK, BT_KNUCKLEBONE, BT_SHRINE, BT_END};

	// 슬롯 위치 계산 (예: 화면 중앙 기준 가로 배치)
	const _float fStartX = 90.f;
	const _float fY = 360.f;
	const _float fGap = 150.f;
	const _float fSlotScale = 100.f;
	
	for (_uint i = 0; i < 4; ++i)
	{
		_float fScreenX = fStartX + i * fGap;
		_float fScreenY = fY;

		_vec3 vPos = ScreenToDX(fScreenX, fScreenY, 0.4f);

		CBuildingSelectSlot* pSlot = CBuildingSelectSlot::Create(m_pGraphicDev, vPos, arrTypes[i]);

		if (nullptr == pSlot)
			return E_FAIL;

		
		pSlot->Set_ScreenPos(fScreenX, fScreenY);
		pSlot->Set_Scale(fSlotScale);

		if (arrTypes[i] == BT_END) pSlot->Set_CanBuild(false);

		m_vecSlots.push_back(pSlot);
	}

	return S_OK;
}

HRESULT CBuildingCraftCtrl::Ready_InfoCard()
{
	m_pInfoCard = CBuildingInfoCard::Create(m_pGraphicDev);

	NULL_CHECK_RETURN(m_pInfoCard, E_FAIL);

	_vec3 vBackPos = ScreenToDX(WINCX * 0.75f, WINCY * 0.5f, 0.5f);
	m_pInfoCard->Set_Pos(CBuildingInfoCard::BACKGROUND, vBackPos);

	_vec3 vIconPos = ScreenToDX(WINCX * 0.75f - 80.f, WINCY * 0.5f - 50.f, 0.4f);
	m_pInfoCard->Set_Pos(CBuildingInfoCard::ICON, vIconPos);

	// 재료1: 카드 하단 좌측
	_vec3 vIng1Pos = ScreenToDX(WINCX * 0.75f - 40.f, WINCY * 0.5f + 50.f, 0.4f);
	m_pInfoCard->Set_Pos(CBuildingInfoCard::INGREDIENT1, vIng1Pos);

	// 재료2: 카드 하단 우측
	_vec3 vIng2Pos = ScreenToDX(WINCX * 0.75f + 40.f, WINCY * 0.5f + 50.f, 0.4f);
	m_pInfoCard->Set_Pos(CBuildingInfoCard::INGREDIENT2, vIng2Pos);

	m_pInfoCard->Set_NameFontPos(WINCX * 0.75f, WINCY * 0.5f - 100.f);
	m_pInfoCard->Set_CostFontPos(WINCX * 0.75f, WINCY * 0.5f + 80.f);

	return S_OK;
}

HRESULT CBuildingCraftCtrl::Ready_TitleFont()
{
	_float fFontScale(0.5f);

	// 폰트 생성
	m_pTitleFont = CFontUIOrtho::Create(m_pGraphicDev);

	NULL_CHECK_RETURN(m_pTitleFont, E_FAIL);

	m_pTitleFont->Set_Font(L"Font_NotoSans30");
	m_pTitleFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pTitleFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pTitleFont->Set_Text(L"건설");
	_vec3 vTitlePos = ScreenToDX(320.f, 80.f, 0.4f);
	m_pTitleFont->Set_Pos(_vec2(vTitlePos.x, vTitlePos.y));
	m_pTitleFont->Set_Scale(_vec2(118.f * fFontScale, 51.f * fFontScale));

	return S_OK;
}

void CBuildingCraftCtrl::Ready_Event()
{

}

void CBuildingCraftCtrl::Update_Select(const _float& fTimeDelta)
{
	// BACKSPACE : 닫기
	if (CDInputMgr::GetInstance()->Key_Down(DIK_BACK))
	{
		Close();
		return;
	}

	// 슬롯 마우스 호버 체크 (내부에서 인포카드 정보 갱신)
	Update_SlotHover();

	CGameObject::Update_GameObject(fTimeDelta);

	// 슬롯 업데이트 및 클릭 체크
	for (auto& pSlot : m_vecSlots)
	{
		pSlot->Update_GameObject(fTimeDelta);

		if (pSlot->Is_Clicked() && pSlot->Get_CanBuild())
		{
			if (pSlot->Get_BuildingType() != BT_END)
			{
				On_SlotClicked(pSlot->Get_BuildingType());
			}
			return;
		}
	}

	// 인포카드 업데이트
	if (m_pInfoCard)
		m_pInfoCard->Update_GameObject(fTimeDelta);

	m_pTitleFont->Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}

void CBuildingCraftCtrl::Update_Build(const _float& fTimeDelta)
{
	// BCS_BUILD 상태일 때 UI는 대기 (설치 완료/취소 시 상태 변경)
}

void CBuildingCraftCtrl::Update_SlotHover()
{
	CBuildingSelectSlot* pNewHovered = nullptr;

	for (auto& pSlot : m_vecSlots)
	{	// 슬롯 별 마우스 호버 상태 갱신
		if (pSlot->Is_Hovered())
		{
			pNewHovered = pSlot;
			pSlot->Set_Hovered(true);
		}
		else
		{
			pSlot->Set_Hovered(false);
		}
	}

	// 호버 대상 변경 시 인포카드 갱신
	if (pNewHovered != m_pHoveredSlot)
	{
		m_pHoveredSlot = pNewHovered;

		if (m_pHoveredSlot)
		{
			BUILDING_TYPE eType = m_pHoveredSlot->Get_BuildingType();

			if (eType != BT_END)	m_pInfoCard->Show(eType);
			else					m_pInfoCard->Hide();
		}
		else m_pInfoCard->Hide();
	}
}

void CBuildingCraftCtrl::On_SlotClicked(BUILDING_TYPE eType)
{
	// UI 닫기
	m_pInfoCard->Hide();
	m_eState = BCS_BUILD;

	// Scene에 이벤트 발행
	IMessageChannel::EVENT tEvent;
	tEvent.strType = L"Building.Select";
	//------------------
	// 이벤트 정보 전달
	//------------------
	m_pMessageChannel->Publish(tEvent);
}

_vec3 CBuildingCraftCtrl::ScreenToDX(const _float& fX, const _float& fY, const _float& fZ)
{
	return _vec3(fX - _float(WINCX) * 0.5f, -fY + _float(WINCY) * 0.5f, fZ);
}

void CBuildingCraftCtrl::DXToScreen(const _vec3& vDX, _float& fScreenX, _float& fScreenY)
{
	fScreenX = vDX.x + _float(WINCX) * 0.5f;
	fScreenY = -vDX.y + _float(WINCY) * 0.5f;
}

CBuildingCraftCtrl* CBuildingCraftCtrl::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CBuildingCraftCtrl* pInstance = new CBuildingCraftCtrl(pGraphicDev);

	pInstance->m_pMessageChannel = pMessageChannel;
	pInstance->m_pMessageChannel->AddRef();

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("pBuildingCraftCtrl Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CBuildingCraftCtrl::Free()
{
	for (auto& pSlot : m_vecSlots)
	{
		Safe_Release(pSlot);
	}

	m_vecSlots.clear();

	Safe_Release(m_pInfoCard);
	Safe_Release(m_pTitleFont);

	CUi::Free();
}
