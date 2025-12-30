#include "pch.h"
#include "CKBTab.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CKBTab::CKBTab(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CKBTab::~CKBTab()
{
}

HRESULT CKBTab::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale((413.f * 0.5f), (100.f * 0.5f), 1.f);
	m_pTransformCom->Set_Pos(0.f, -272.f, 0.f);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);

	return S_OK;
}

_int CKBTab::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CKBTab::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CKBTab::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CKBTab::OnCollision(CGameObject* pObject)
{

}

void CKBTab::Move_Down()
{
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y - 60.f, m_vPos.z);
}

void CKBTab::Move_Up()
{
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y + 60.f, m_vPos.z);
}

void CKBTab::Move_Title()
{
	m_pTransformCom->Set_Pos(0.f, -152.f, 0.f);
}

HRESULT CKBTab::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_KBTab"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CKBTab* CKBTab::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKBTab* pKBCenter = new CKBTab(pGraphicDev);

	if (FAILED(pKBCenter->Ready_GameObject()))
	{
		Safe_Release(pKBCenter);
		MSG_BOX("pKBCenter Create Failed");
		return nullptr;
	}

	return pKBCenter;
}

void CKBTab::Free()
{
	CUi::Free();
}