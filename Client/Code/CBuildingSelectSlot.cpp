#include "pch.h"
#include "CBuildingSelectSlot.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"

CBuildingSelectSlot::CBuildingSelectSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
}

CBuildingSelectSlot::~CBuildingSelectSlot()
{
}

HRESULT	CBuildingSelectSlot::Ready_GameObject()
{
	return S_OK;
}

_int CBuildingSelectSlot::Update_GameObject(const _float& fTimeDelta)
{


	return NOEVENT;
}

void CBuildingSelectSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CBuildingSelectSlot::Render_GameObject()
{
}

_bool CBuildingSelectSlot::Is_Clicked()
{
	return Is_Hovered() && CDInputMgr::GetInstance()->Mouse_Down(DIM_LB);
}

_bool CBuildingSelectSlot::Is_Hovered()
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	// 슬롯 영역 계산 (스크린 좌표)
	_float fLeft	= m_vPos.x - m_fWidth * 0.5f;
	_float fRight	= m_vPos.x + m_fWidth * 0.5f;
	_float fTop		= m_vPos.y - m_fHeight * 0.5f;
	_float fBottom	= m_vPos.y + m_fHeight * 0.5f;

	return	(ptMouse.x >= fLeft	&& ptMouse.x <= fRight &&
			 ptMouse.y >= fTop	&& ptMouse.y <= fBottom);
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingSelectSlot"));

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
	pInstance->m_eBuildingType = eType;

	return pInstance;
}

void CBuildingSelectSlot::Free()
{
	CUi::Free();
}
