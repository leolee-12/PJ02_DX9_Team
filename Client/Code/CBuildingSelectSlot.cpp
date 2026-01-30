#include "pch.h"
#include "CBuildingSelectSlot.h"
#include "CBuilding.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"
#include "CPersistentMgr.h"
#include "CInventory.h"

CBuildingSelectSlot::CBuildingSelectSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pSlotTextureCom(nullptr)
	, m_pIconTextureCom(nullptr)
	, m_eBuildingType(BT_END)
	, m_bHovered(false)
	, m_bCanBuild(true)
	, m_fScreenX(0.f)
	, m_fScreenY(0.f)
	, m_fBaseScale(1.f)
	, m_fTargetScale(1.f)
	, m_fCurScale(1.f)
{
}

CBuildingSelectSlot::~CBuildingSelectSlot()
{
}

void CBuildingSelectSlot::Set_Scale(const _float& fScale)
{
	m_pTransformCom->Set_Scale(fScale, fScale, 1.f);
	m_fBaseScale = m_fCurScale = m_fTargetScale = fScale;
}

HRESULT	CBuildingSelectSlot::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CBuildingSelectSlot::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bHovered)
	{
		_float fMultiplier = 1.15f;
		m_fTargetScale = m_fBaseScale * fMultiplier;
	}
	else m_fTargetScale = m_fBaseScale;

	const _float fLerp = 10.f;

	m_fCurScale = m_fCurScale + (m_fTargetScale - m_fCurScale) * fTimeDelta * fLerp;

	m_pTransformCom->Set_Scale(m_fCurScale, m_fCurScale, 1.f);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CBuildingSelectSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CBuildingSelectSlot::Render_GameObject()
{
	// 슬롯 배경 렌더
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pSlotTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	// BT_END는 아이콘 렌더 안함 (텍스처 미싱 방지)
	if (m_eBuildingType != BT_END)
	{
		m_pIconTextureCom->Set_Texture(_uint(m_eBuildingType - 2));	// DUMMY, WORKSHOP 제외
		m_pBufferCom->Render_Buffer();
	}
}

void CBuildingSelectSlot::Check_CanBuild()
{
	CInventory* pInventory = CPersistentMgr::GetInstance()->Get_Inventory();

	switch (m_eBuildingType)
	{
	case BT_COOK:
		m_bCanBuild = (pInventory->Get_ItemCount(CItem::IG_WOOD) >= 2 && pInventory->Get_ItemCount(CItem::IG_STONE) >= 2);
		break;

	case BT_KNUCKLEBONE:
		m_bCanBuild = (pInventory->Get_ItemCount(CItem::IG_GOLD) >= 2 && pInventory->Get_ItemCount(CItem::IG_WOOD) >= 2);
		break;

	case BT_SHRINE:
		m_bCanBuild = (pInventory->Get_ItemCount(CItem::IG_GOLD) >= 2 && pInventory->Get_ItemCount(CItem::IG_STONE) >= 2);
		break;
	}
}

_bool CBuildingSelectSlot::Check_Clicked()
{
	return m_bHovered && CDInputMgr::GetInstance()->Mouse_Down(DIM_LB);
}

_bool CBuildingSelectSlot::Check_Hovered()
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	const _float fHalfSize = 50.f * (m_fCurScale / m_fBaseScale);

	_float fLeft = m_fScreenX - fHalfSize;
	_float fRight = m_fScreenX + fHalfSize;
	_float fTop = m_fScreenY - fHalfSize;
	_float fBottom = m_fScreenY + fHalfSize;

	return (ptMouse.x >= fLeft && ptMouse.x <= fRight &&
			ptMouse.y >= fTop && ptMouse.y <= fBottom);
}

HRESULT	CBuildingSelectSlot::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform_BG", pComponent });

	// Texture
	pComponent = m_pSlotTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BuildingIconBack"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Slot", pComponent });

	pComponent = m_pIconTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BuildingIcon"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture_Icon", pComponent });

	return S_OK;
}

CBuildingSelectSlot* CBuildingSelectSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, BUILDING_TYPE eType)
{
	CBuildingSelectSlot* pInstance = new CBuildingSelectSlot(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("pBuildingSelectSlot Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	pInstance->m_pTransformCom->Set_Scale(pInstance->m_fBaseScale, pInstance->m_fBaseScale, 1.f);
	pInstance->m_eBuildingType = eType;


	return pInstance;
}

void CBuildingSelectSlot::Free()
{
	CUi::Free();
}
