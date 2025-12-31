#include "pch.h"
#include "CDivider.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CDivider::CDivider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CDivider::~CDivider()
{
}

HRESULT CDivider::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (m_iPage > 2 || m_iPage < 0)
	{
		MSG_BOX("CDivider Page Error");
		return E_FAIL;
	}

	m_pTransformCom->Set_Scale(121.f * 0.7f, 55.f * 0.7f, 1.f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	return S_OK;
}

_int CDivider::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CDivider::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CDivider::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iPage);

	m_pBufferCom->Render_Buffer();
}

void CDivider::OnCollision(CGameObject* pObject)
{

}

HRESULT CDivider::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Divider"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CDivider* CDivider::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iPage, const _vec3& vPos)
{
	CDivider* pDivider = new CDivider(pGraphicDev);
	pDivider->m_iPage = iPage;
	pDivider->m_vPos = vPos;

	if (FAILED(pDivider->Ready_GameObject()))
	{
		Safe_Release(pDivider);
		MSG_BOX("pDivider Create Failed");
		return nullptr;
	}

	return pDivider;
}

void CDivider::Free()
{
	CUi::Free();
}