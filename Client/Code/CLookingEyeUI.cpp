#include "pch.h"
#include "CLookingEyeUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CLookingEyeUI::CLookingEyeUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CLookingEyeUI::~CLookingEyeUI()
{
}

HRESULT CLookingEyeUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(204.0f * m_fScale, 117.0f * m_fScale, 1.0f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	return S_OK;
}

_int CLookingEyeUI::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CLookingEyeUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CLookingEyeUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CLookingEyeUI::OnCollision(CGameObject* pObject)
{

}

HRESULT CLookingEyeUI::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_NPCCommaderUIEye"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


CLookingEyeUI* CLookingEyeUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale)
{
	CLookingEyeUI* pCLookingEyeUI = new CLookingEyeUI(pGraphicDev);

	pCLookingEyeUI->m_vPos = _vPos;
	pCLookingEyeUI->m_fScale = _fScale;

	if (FAILED(pCLookingEyeUI->Ready_GameObject()))
	{
		Safe_Release(pCLookingEyeUI);
		MSG_BOX("pCLookingEyeUI Create Failed");
		return nullptr;
	}
	return pCLookingEyeUI;
}

void CLookingEyeUI::Free()
{
	CUi::Free();
}
