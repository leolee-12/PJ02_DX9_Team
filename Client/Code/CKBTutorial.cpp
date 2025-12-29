#include "pch.h"
#include "CKBTutorial.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CKBTutorial::CKBTutorial(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CKBTutorial::~CKBTutorial()
{
}

HRESULT CKBTutorial::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	switch (m_iPage) {
	case 0:
		m_pTransformCom->Set_Scale(256.f, 256.f, 1.f);
		m_pTransformCom->Set_Pos(-250.f, 40.f, 0.25f);
		break;
	case 1:
		m_pTransformCom->Set_Scale(389.f * 0.5f, 464.f * 0.5f, 1.f);
		m_pTransformCom->Set_Pos(250.f, 40.f, 0.25f);
		break;
	default:
		MSG_BOX("튜토리얼ui 페이지값 오류");
		return E_FAIL;
	}

	return S_OK;
}

_int CKBTutorial::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CKBTutorial::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CKBTutorial::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iPage);

	m_pBufferCom->Render_Buffer();
}

void CKBTutorial::OnCollision(CGameObject* pObject)
{

}

HRESULT CKBTutorial::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TTutorial"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CKBTutorial* CKBTutorial::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iPage)
{
	CKBTutorial* pKBTutorial = new CKBTutorial(pGraphicDev);
	pKBTutorial->m_iPage = iPage;

	if (FAILED(pKBTutorial->Ready_GameObject()))
	{
		Safe_Release(pKBTutorial);
		MSG_BOX("pKBTutorial Create Failed");
		return nullptr;
	}

	return pKBTutorial;
}

void CKBTutorial::Free()
{
	CUi::Free();
}