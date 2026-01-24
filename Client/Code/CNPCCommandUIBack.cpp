#include "pch.h"
#include "CNPCCommandUIBack.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CNPCCommandUIBack::CNPCCommandUIBack(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CNPCCommandUIBack::~CNPCCommandUIBack()
{
}

HRESULT CNPCCommandUIBack::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(128.0f * m_fScale, 128.0f * m_fScale, 1.0f);
	m_pTransformCom->Set_Pos(m_vPos.x,m_vPos.y, m_vPos.z);

	return S_OK;
}

_int CNPCCommandUIBack::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CNPCCommandUIBack::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CNPCCommandUIBack::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CNPCCommandUIBack::OnCollision(CGameObject* pObject)
{

}

HRESULT CNPCCommandUIBack::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_NPCCommaderUIBack"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


CNPCCommandUIBack* CNPCCommandUIBack::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale)
{
	CNPCCommandUIBack* pCNPCCommandUIBack = new CNPCCommandUIBack(pGraphicDev);

	pCNPCCommandUIBack->m_vPos = _vPos;
	pCNPCCommandUIBack->m_fScale = _fScale;


	if (FAILED(pCNPCCommandUIBack->Ready_GameObject()))
	{
		Safe_Release(pCNPCCommandUIBack);
		MSG_BOX("pCNPCCommandUIBack Create Failed");
		return nullptr;
	}
	return pCNPCCommandUIBack;
}

void CNPCCommandUIBack::Free()
{
	CUi::Free();
}
