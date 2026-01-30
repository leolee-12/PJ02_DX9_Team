#include "pch.h"
#include "CBuildingInfoCard.h"
#include "CProtoMgr.h"
#include "CBuilding.h"
#include "CFontUIOrtho.h"
#include "CItem.h"
#include "CPersistentMgr.h"

CBuildingInfoCard::CBuildingInfoCard(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pFont(nullptr)
	, m_bVisible(false)
	, m_eDisplayType(BT_END)
{
	ZeroMemory(m_pTransformCom, sizeof(m_pTransformCom));
	ZeroMemory(m_pTextureCom, sizeof(m_pTextureCom));
	ZeroMemory(m_iTexIdx, sizeof(m_iTexIdx));
}

CBuildingInfoCard::~CBuildingInfoCard()
{
}

void CBuildingInfoCard::Set_Pos(TEXTURE_TYPE eType, const _vec3& vPos)
{
	m_pTransformCom[eType]->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CBuildingInfoCard::Set_Scale(TEXTURE_TYPE eType, const _vec3& vScale)
{
	m_pTransformCom[eType]->Set_Scale(vScale.x, vScale.y, vScale.z);
}

void CBuildingInfoCard::Set_Reserve(const _uint& iReserve1, const _uint& iReserve2)
{
	_tchar szBuf[32];

	swprintf_s(szBuf, L"(%d)", iReserve1);
	m_tFontInfo[FID_RESERVE1].strText = szBuf;

	swprintf_s(szBuf, L"(%d)", iReserve2);
	m_tFontInfo[FID_RESERVE2].strText = szBuf;
}

void CBuildingInfoCard::Set_FontPos(FONT_ID eID, const _float& fX, const _float& fY)
{
	m_tFontInfo[eID].vPos = _vec2(fX - WINCX * 0.5f, WINCY * 0.5f - fY);
}

HRESULT	CBuildingInfoCard::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Ready_MyFont()))
		return E_FAIL;

	return S_OK;
}

_int CBuildingInfoCard::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	//m_pFont->Update_GameObject(fTimeDelta);	// 렌더 시에 갱신하므로 불필요

	return iExit;
}

void CBuildingInfoCard::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CBuildingInfoCard::Render_GameObject()
{
	if (!m_bVisible)
		return;

	// 이상없을 경우 반복문으로 변경
	// BG 렌더
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[BACKGROUND]->Get_World());
	m_pTextureCom[BACKGROUND]->Set_Texture(m_iTexIdx[BACKGROUND]);
	m_pBufferCom->Render_Buffer();

	// 아이콘 BG 렌더
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[ICONBACK]->Get_World());
	m_pTextureCom[ICONBACK]->Set_Texture(m_iTexIdx[ICONBACK]);
	m_pBufferCom->Render_Buffer();

	// 건물 아이콘 렌더
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[ICON]->Get_World());
	m_pTextureCom[ICON]->Set_Texture(m_iTexIdx[ICON]);
	m_pBufferCom->Render_Buffer();

	// 재료1 렌더
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[INGREDIENT1]->Get_World());
	m_pTextureCom[INGREDIENT1]->Set_Texture(m_iTexIdx[INGREDIENT1]);
	m_pBufferCom->Render_Buffer();

	// 재료2 렌더
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[INGREDIENT2]->Get_World());
	m_pTextureCom[INGREDIENT2]->Set_Texture(m_iTexIdx[INGREDIENT2]);
	m_pBufferCom->Render_Buffer();

	// 폰트 렌더
	if (m_pFont)
	{
		for (_uint i = 0; i < FID_END; ++i)
		{
			const FONTINFO& tInfo = m_tFontInfo[i];
			m_pFont->Set_Font(tInfo.strFontKey.c_str());
			m_pFont->Set_Pos(tInfo.vPos);
			m_pFont->Set_Scale(tInfo.vScale);
			m_pFont->Set_FontColor(tInfo.tColor);
			m_pFont->Set_Flags(tInfo.uFlags);
			m_pFont->Set_Text(tInfo.strText.c_str());
			m_pFont->Update_GameObject(0.f);	// 폰트 Pos, Scale이 업데이트에서 반영됨 : 업데이트 없이 반복문 돌리면 모두 마지막꺼 위치/크기로 나옴
			m_pFont->Render_GameObject();
		}
	}
}

void CBuildingInfoCard::Show(BUILDING_TYPE eType)
{
	m_bVisible = true;
	m_eDisplayType = eType;
	Update_Content(eType);
}

void CBuildingInfoCard::Hide()
{
	m_bVisible = false;
}

HRESULT	CBuildingInfoCard::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	for (_uint i = 0; i < TT_END; ++i)
	{
		pComponent = m_pTransformCom[i] = dynamic_cast<Engine::CTransform*>
			(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

		NULL_CHECK_RETURN(pComponent, E_FAIL);

		wstring strKey;

		if		(i == BACKGROUND)	strKey = L"Com_Transform_BG";
		else if (i == ICON)			strKey = L"Com_Transform_Icon";
		else if (i == INGREDIENT1)	strKey = L"Com_Transform_IG1";
		else if (i == INGREDIENT2)	strKey = L"Com_Transform_IG2";

		m_mapComponent[ID_DYNAMIC].insert({ strKey, pComponent });
	}

	// Texture
	pComponent = m_pTextureCom[BACKGROUND] = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingCardInfoCardBack"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_BG", pComponent });

	pComponent = m_pTextureCom[ICONBACK] = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BuildingIconBack"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_IconBack", pComponent });

	pComponent = m_pTextureCom[ICON] = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BuildingIcon"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Icon", pComponent });

	pComponent = m_pTextureCom[INGREDIENT1] = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ItemTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_IG1", pComponent });

	pComponent = m_pTextureCom[INGREDIENT2] = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ItemTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_IG2", pComponent });

	return S_OK;
}

HRESULT CBuildingInfoCard::Ready_MyFont()
{
	// 폰트 생성
	m_pFont = CFontUIOrtho::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(m_pFont, E_FAIL);

	m_tFontInfo[FID_NAME]		= { {0.f, 0.f}, {300.f, 30.f},	{1.0f, 1.0f, 1.0f, 1.f},	DT_CENTER | DT_VCENTER, L"", L"Font_Default30_Heavy" };
	m_tFontInfo[FID_DATA]		= { {0.f, 0.f}, {300.f, 48.f},	{0.75f, 0.75f, 0.75f, 1.f},	DT_CENTER | DT_VCENTER | DT_WORDBREAK, L"", L"Font_Default24" };
	m_tFontInfo[FID_COST1]		= { {0.f, 0.f}, {60.f, 20.f},	{1.0f, 1.0f, 1.0f, 1.f},	DT_RIGHT | DT_VCENTER, L"", L"Font_Default" };
	m_tFontInfo[FID_RESERVE1]	= { {0.f, 0.f}, {60.f, 20.f},	{0.75f, 0.75f, 0.75f, 1.f},	DT_RIGHT | DT_VCENTER, L"", L"Font_Default" };
	m_tFontInfo[FID_COST2]		= { {0.f, 0.f}, {60.f, 20.f},	{1.0f, 1.0f, 1.0f, 1.f},	DT_RIGHT | DT_VCENTER, L"", L"Font_Default" };
	m_tFontInfo[FID_RESERVE2]	= { {0.f, 0.f}, {60.f, 20.f},	{0.75f, 0.75f, 0.75f, 1.f},	DT_RIGHT | DT_VCENTER, L"", L"Font_Default" };

	return S_OK;
}

void CBuildingInfoCard::Update_Content(BUILDING_TYPE eType)
{
	// 건물 타입별 정보 설정
	switch (eType)
	{
	case BT_COOK:
		m_iTexIdx[INGREDIENT1] = CItem::IG_WOOD;
		m_iTexIdx[INGREDIENT2] = CItem::IG_STONE;
		m_tFontInfo[FID_NAME]	.strText = L"요리용 화덕";
		m_tFontInfo[FID_DATA]	.strText = L"날 것을 요리해 먹을 수 있는 요리로 만듭니다.";
		m_tFontInfo[FID_COST1]	.strText = L"나무 x2";
		m_tFontInfo[FID_COST2]	.strText = L"돌 x2";
		Set_Reserve(CPersistentMgr::GetInstance()->Get_Inventory()->Get_ItemCount(CItem::IG_WOOD),
					CPersistentMgr::GetInstance()->Get_Inventory()->Get_ItemCount(CItem::IG_STONE));
		break;
	case BT_KNUCKLEBONE:
		m_iTexIdx[INGREDIENT1] = CItem::IG_GOLD;
		m_iTexIdx[INGREDIENT2] = CItem::IG_WOOD;
		m_tFontInfo[FID_NAME]	.strText = L"너클본 테이블";
		m_tFontInfo[FID_DATA]	.strText = L"라타우가 좋아하는 너클본 게임을 할 수 있습니다.";
		m_tFontInfo[FID_COST1]	.strText = L"금화 x2";
		m_tFontInfo[FID_COST2]	.strText = L"나무 x2";
		Set_Reserve(CPersistentMgr::GetInstance()->Get_Inventory()->Get_ItemCount(CItem::IG_GOLD),
					CPersistentMgr::GetInstance()->Get_Inventory()->Get_ItemCount(CItem::IG_WOOD));

		break;
	case BT_SHRINE:
		m_iTexIdx[INGREDIENT1] = CItem::IG_GOLD;
		m_iTexIdx[INGREDIENT2] = CItem::IG_STONE;
		m_tFontInfo[FID_NAME].strText = L"성지";
		m_tFontInfo[FID_DATA].strText = L"추종자들이 기도를 드리는 곳, 신앙 게이지를 축적합니다.";
		m_tFontInfo[FID_COST1].strText = L"금화 x2";
		m_tFontInfo[FID_COST2].strText = L"돌 x2";
		Set_Reserve(CPersistentMgr::GetInstance()->Get_Inventory()->Get_ItemCount(CItem::IG_GOLD),
					CPersistentMgr::GetInstance()->Get_Inventory()->Get_ItemCount(CItem::IG_STONE));
		break;
	case BT_END:
		m_tFontInfo[FID_NAME].strText = L"";
		m_tFontInfo[FID_DATA].strText = L"";
		m_tFontInfo[FID_COST1].strText = L"";
		m_tFontInfo[FID_RESERVE1].strText = L"";
		m_tFontInfo[FID_COST2].strText = L"";
		m_tFontInfo[FID_RESERVE2].strText = L"";
		break;
	}

	m_iTexIdx[BACKGROUND] = 0;
	m_iTexIdx[ICON] = eType - 2;	// DUMMY, WORKSHOP 제외

	// 폰트 활성화
	m_pFont->Active();
}

CBuildingInfoCard* CBuildingInfoCard::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBuildingInfoCard* pInstance = new CBuildingInfoCard(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("pBuildingInfoCard Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CBuildingInfoCard::Free()
{
	Safe_Release(m_pFont);

	CUi::Free();
}
