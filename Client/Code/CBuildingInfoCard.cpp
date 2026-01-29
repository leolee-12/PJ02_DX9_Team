#include "pch.h"
#include "CBuildingInfoCard.h"
#include "CProtoMgr.h"
#include "CBuilding.h"
#include "CFontUIOrtho.h"
#include "CItem.h"

CBuildingInfoCard::CBuildingInfoCard(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pNameFont(nullptr)
	, m_pCostFont(nullptr)
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

void CBuildingInfoCard::Set_NameFontPos(const _float& fX, const _float& fY)
{
	m_pNameFont->Set_Pos(_vec2(fX - WINCX * 0.5f, WINCY * 0.5f - fY));
}

void CBuildingInfoCard::Set_CostFontPos(const _float& fX, const _float& fY)
{
	m_pCostFont->Set_Pos(_vec2(fX - WINCX * 0.5f, WINCY * 0.5f - fY));
}

HRESULT	CBuildingInfoCard::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Ready_MyTransform()))
		return E_FAIL;

	if (FAILED(Ready_MyFont()))
		return E_FAIL;

	return S_OK;
}

_int CBuildingInfoCard::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_pNameFont->Update_GameObject(fTimeDelta);
	m_pCostFont->Update_GameObject(fTimeDelta);

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
	// 배경 렌더
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[BACKGROUND]->Get_World());
	m_pTextureCom[BACKGROUND]->Set_Texture(m_iTexIdx[BACKGROUND]);
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

	// 폰트 렌더 (Update에서 Active된 경우)
	if (m_pNameFont)
		m_pNameFont->Render_GameObject();
	if (m_pCostFont)
		m_pCostFont->Render_GameObject();
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

HRESULT CBuildingInfoCard::Ready_MyTransform()
{
	// 스케일 설정 (Ctrl이 위치 설정)
	m_pTransformCom[BACKGROUND]->Set_Scale(200.f, 300.f, 1.f);
	m_pTransformCom[ICON]->Set_Scale(64.f, 64.f, 1.f);
	m_pTransformCom[INGREDIENT1]->Set_Scale(32.f, 32.f, 1.f);
	m_pTransformCom[INGREDIENT2]->Set_Scale(32.f, 32.f, 1.f);

	return S_OK;
}

HRESULT CBuildingInfoCard::Ready_MyFont()
{
	// 폰트 생성
	m_pNameFont = CFontUIOrtho::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(m_pNameFont, E_FAIL);
	m_pNameFont->Set_Font(L"Font_NotoSans30");
	m_pNameFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pNameFont->Set_Flags(DT_CENTER | DT_VCENTER);

	m_pCostFont = CFontUIOrtho::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(m_pCostFont, E_FAIL);
	m_pCostFont->Set_Font(L"Font_NotoSans30");
	m_pCostFont->Set_FontColor(D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f));
	m_pCostFont->Set_Flags(DT_CENTER | DT_VCENTER);

	return S_OK;
}

void CBuildingInfoCard::Update_Content(BUILDING_TYPE eType)
{
	// 건물 타입별 정보 설정
	switch (eType)
	{
	case BT_COOK:
		m_pNameFont->Set_Text(L"오븐");
		m_pCostFont->Set_Text(L"나무 x3, 돌 x3");
		m_iTexIdx[INGREDIENT1] = CItem::IG_WOOD;
		m_iTexIdx[INGREDIENT2] = CItem::IG_STONE;
		break;
	case BT_KNUCKLEBONE:
		m_pNameFont->Set_Text(L"너클본테이블");
		m_pCostFont->Set_Text(L"금화 x3, 나무 x3");
		m_iTexIdx[INGREDIENT1] = CItem::IG_GOLD;
		m_iTexIdx[INGREDIENT2] = CItem::IG_WOOD;
		break;
	case BT_SHRINE:
		m_pNameFont->Set_Text(L"성지");
		m_pCostFont->Set_Text(L"금화 x3, 돌 x3");
		m_iTexIdx[INGREDIENT1] = CItem::IG_GOLD;
		m_iTexIdx[INGREDIENT2] = CItem::IG_STONE;
		break;
	case BT_END:
		m_pNameFont->Set_Text(L"");
		m_pCostFont->Set_Text(L"");
		break;
	}

	m_iTexIdx[BACKGROUND] = 0;
	m_iTexIdx[ICON] = eType - 2;	// DUMMY, WORKSHOP 제외

	// 폰트 활성화
	m_pNameFont->Active();
	m_pCostFont->Active();
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
	Safe_Release(m_pNameFont);
	Safe_Release(m_pCostFont);

	CUi::Free();
}
