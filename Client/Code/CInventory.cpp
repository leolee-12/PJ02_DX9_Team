#include "pch.h"
#include "CInventory.h"

#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"
#include "CPersistentMgr.h"

#include "CInvenBack.h"
#include "CInvenSlot.h"
#include "CInvenItem.h"
#include "CInvenBtn.h"


CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
}

CInventory::~CInventory()
{

}

HRESULT CInventory::Ready_GameObject()
{
	m_pInvenBack = CInvenBack::Create(m_pGraphicDev,_vec3(100.0f,100.0f,0.1f),1.0f);
	if (nullptr == m_pInvenBack)
		return E_FAIL;

	m_pInvenBtn = CInvenBtn::Create(m_pGraphicDev, _vec3(150.0f, 100.0f, 0.1f), 1.0f);
	if (nullptr == m_pInvenBtn)
		return E_FAIL;

	m_pInvenItem = CInvenItem::Create(m_pGraphicDev, _vec3(200.0f, 100.0f, 0.1f), 1.0f);
	if (nullptr == m_pInvenItem)
		return E_FAIL;

	m_pInvenSlot = CInvenSlot::Create(m_pGraphicDev, _vec3(250.0f, 100.0f, 0.1f), 1.0f);
	if (nullptr == m_pInvenSlot)
		return E_FAIL;

	return S_OK;
}

_int CInventory::Update_GameObject(const _float& fTimeDelta)
{
	Key_Input_Inven();
	if (!m_bActive) { return NOEVENT; }

	m_pInvenBtn->Update_GameObject(fTimeDelta);
	m_pInvenBack->Update_GameObject(fTimeDelta);
	m_pInvenItem->Update_GameObject(fTimeDelta);
	m_pInvenSlot->Update_GameObject(fTimeDelta);
	return NOEVENT;
}

void CInventory::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bActive) { return; }
	m_pInvenBtn->LateUpdate_GameObject(fTimeDelta);
	m_pInvenBack->LateUpdate_GameObject(fTimeDelta);
	m_pInvenItem->LateUpdate_GameObject(fTimeDelta);
	m_pInvenSlot->LateUpdate_GameObject(fTimeDelta);
}

void CInventory::Render_GameObject()
{
	if (!m_bActive) { return; }
	m_pInvenBtn->Render_GameObject();
	m_pInvenBack->Render_GameObject();
	m_pInvenItem->Render_GameObject();
	m_pInvenSlot->Render_GameObject();
}

void CInventory::Key_Input_Inven()
{
	if (CDInputMgr::GetInstance()->Key_Down(DIK_L))
	{
		m_bActive = !m_bActive;
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_Q))
	{
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_R))
	{
	}
}

void CInventory::Ready_Event()
{
}

CInventory* CInventory::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CInventory* pInstance = new CInventory(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CInventory Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CInventory::Free()
{

	CUi::Free();
}

