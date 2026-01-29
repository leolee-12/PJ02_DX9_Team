#include "pch.h"
#include "CBuildingSelectSlot.h"
#include "CBuilding.h"
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

	m_fCurrentScale = m_fCurrentScale + (m_fTargetScale - m_fCurrentScale) * fTimeDelta * fLerp;

	m_pTransformCom->Set_Scale(m_fCurrentScale, m_fCurrentScale, 1.f);

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
		m_pIconTextureCom->Set_Texture((_uint)m_eBuildingType);
		m_pBufferCom->Render_Buffer();
	}
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
	_float fHalfW = m_fWidth * m_fCurrentScale * 0.5f;
	_float fHalfH = m_fHeight * m_fCurrentScale * 0.5f;

	return (ptMouse.x >= m_fScreenX - fHalfW && ptMouse.x <= m_fScreenX + fHalfW &&
			ptMouse.y >= m_fScreenY - fHalfH && ptMouse.y <= m_fScreenY + fHalfH);
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
