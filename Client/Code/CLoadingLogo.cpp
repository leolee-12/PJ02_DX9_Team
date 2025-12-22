#include "pch.h"
#include "CLoadingLogo.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CLoadingLogo::CLoadingLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CLoadingLogo::~CLoadingLogo()
{
}

HRESULT CLoadingLogo::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(100.f, 100.f, 1.f);
	m_pTransformCom->Set_Pos(_float(-WINCX / 2) + 100.f, _float(-WINCY / 2) + 100.f, 0.f);

	return S_OK;
}

_int CLoadingLogo::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CLoadingLogo::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	//m_pTransformCom->Rotation(ROT_Z, 0.5f);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CLoadingLogo::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
}

void CLoadingLogo::OnCollision(CGameObject* pObject)
{

}

HRESULT CLoadingLogo::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LoadingLogoTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CLoadingLogo* CLoadingLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLoadingLogo* pLodingBack = new CLoadingLogo(pGraphicDev);

	if (FAILED(pLodingBack->Ready_GameObject()))
	{
		Safe_Release(pLodingBack);
		MSG_BOX("pLodingBack Create Failed");
		return nullptr;
	}

	return pLodingBack;
}

void CLoadingLogo::Update_Pos(_float LoadingPercent)
{
	_float fScaleY = m_pTransformCom->Get_World()->m[1][1];
	_float fTargetY = LoadingPercent * fScaleY;
	_float flerpY = m_vPos.y + (fTargetY - m_vPos.y) * 0.1;
	m_pTransformCom->Set_Pos(m_vPos.x, flerpY, m_vPos.z);
}

void CLoadingLogo::Free()
{
	CUi::Free();
}