#include "pch.h"
#include "CBuildingCraftCtrl.h"
#include "CBuildingInfoCard.h"
#include "CBuildingSelectSlot.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"
#include "CFontMgr.h"

CBuildingCraftCtrl::CBuildingCraftCtrl(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
}

CBuildingCraftCtrl::~CBuildingCraftCtrl()
{
}

HRESULT	CBuildingCraftCtrl::Ready_GameObject()
{
	if(FAILED(Add_Component()))
		return E_FAIL;

	_vec3 vBackPos = ScreenToDX(WINCX * 0.25f, WINCY * 0.5f, 0.5f);
	m_pTransformCom[BACKGROUND]->Set_Pos(vBackPos.x, vBackPos.y, vBackPos.z);
	m_pTransformCom[BACKGROUND]->Set_Scale(WINCX * 0.5f, WINCY, 1.f);

	_vec3 vDeco1Pos = ScreenToDX(120.f, 80.f, 0.4f);
	m_pTransformCom[DECO1]->Set_Pos(vDeco1Pos.x, vDeco1Pos.y, vDeco1Pos.z);
	m_pTransformCom[DECO1]->Set_Scale(50.f, 50.f, 1.f);

	_vec3 vDeco2Pos = ScreenToDX(280.f, 80.f, 0.4f);
	m_pTransformCom[DECO2]->Set_Pos(vDeco2Pos.x, vDeco2Pos.y, vDeco2Pos.z);
	m_pTransformCom[DECO2]->Set_Scale(50.f, 50.f, 1.f);

	if (FAILED(Ready_Slots()))
		return E_FAIL;

	if (FAILED(Ready_InfoCard()))
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

	// 반복문 가능한데 헷갈려서 명시
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

	// 타이틀 폰트 (CFontMgr 직접 호출)
	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	RECT rcTitle = { 0, 0, WINCX / 2, WINCY / 2 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", L"건설", rcTitle, FontColor, DT_CENTER | DT_BOTTOM);

	// 슬롯 렌더
	for (auto& pSlot : m_vecSlots)
		pSlot->Render_GameObject();

	// 인포카드 렌더
	if (m_pInfoCard)
		m_pInfoCard->Render_GameObject();
}

void CBuildingCraftCtrl::Open()
{
}

void CBuildingCraftCtrl::Close()
{
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

HRESULT CBuildingCraftCtrl::Ready_Slots()
{
	BUILDING_TYPE arrTypes[4] = { BT_COOK, BT_KNUCKLEBONE, BT_SHRINE, BT_END};

	// 슬롯 위치 계산 (예: 화면 중앙 기준 가로 배치)
	const _float fStartX = 60.f;
	const _float fY = 450.f;
	const _float fGap = 100.f;
	
	for (_uint i = 0; i < 4; ++i)
	{
		_vec3 vPos = ScreenToDX(fStartX + i * fGap, fY, 0.4f);

		CBuildingSelectSlot* pSlot = CBuildingSelectSlot::Create(m_pGraphicDev, vPos, arrTypes[i]);

		if (nullptr == pSlot)
			return E_FAIL;

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

			if (eType != BT_END) m_pInfoCard->Show(eType);
		}
		else m_pInfoCard->Hide();
	}
	else m_pInfoCard->Hide();
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
	fScreenY = vDX.y + _float(WINCY) * 0.5f;
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
	CUi::Free();
}
