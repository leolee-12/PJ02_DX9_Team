#include "pch.h"
#include "CBuildingInfoCard.h"
#include "CProtoMgr.h"
#include "CBuilding.h"
#include "CFontUIOrtho.h"

CBuildingInfoCard::CBuildingInfoCard(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
}

CBuildingInfoCard::~CBuildingInfoCard()
{
}

HRESULT	CBuildingInfoCard::Ready_GameObject()
{
	return S_OK;
}

_int CBuildingInfoCard::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CBuildingInfoCard::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CBuildingInfoCard::Render_GameObject()
{
}

void CBuildingInfoCard::Show(BUILDING_TYPE eType, const _vec3& vSlotPos)
{
	m_bVisible = true;
	m_eDisplayType = eType;

	// 슬롯 위쪽에 위치
	_vec3 vPos = vSlotPos;
	vPos.y -= 80.f;  // 슬롯 위로 오프셋
	//m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

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

void CBuildingInfoCard::Update_Content(BUILDING_TYPE eType)
{
	// 건물 타입별 정보 설정
	switch (eType)
	{
	case BT_COOK:
		m_pNameFont->Set_Text(L"오븐");
		m_pCostFont->Set_Text(L"나무 x5");
		break;
	case BT_KNUCKLEBONE:
		m_pNameFont->Set_Text(L"너클본테이블");
		m_pCostFont->Set_Text(L"나무 x3, 돌 x2");
		break;
	case BT_SHRINE:
		m_pNameFont->Set_Text(L"성지");
		m_pCostFont->Set_Text(L"나무 x3, 돌 x2");
		break;
	case BT_END:
		m_pNameFont->Set_Text(L"");
		m_pCostFont->Set_Text(L"");
		break;
	}
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
	CUi::Free();
}
