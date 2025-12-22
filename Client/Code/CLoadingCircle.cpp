#include "pch.h"
#include "CLoadingCircle.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CLoadingCircle::CLoadingCircle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CLoadingCircle::~CLoadingCircle()
{
}

HRESULT CLoadingCircle::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(25.f, 25.f, 1.f);
	m_pTransformCom->Set_Pos(_float(-WINCX / 2) + 275.f, _float(-WINCY / 2) + 80.f, 0.f);

	return S_OK;
}

_int CLoadingCircle::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CLoadingCircle::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Rotation(ROT_Z, 5.f);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CLoadingCircle::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
}

void CLoadingCircle::OnCollision(CGameObject* pObject)
{

}

HRESULT CLoadingCircle::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LoadingCircleTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CLoadingCircle* CLoadingCircle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLoadingCircle* pLodingBack = new CLoadingCircle(pGraphicDev);

	if (FAILED(pLodingBack->Ready_GameObject()))
	{
		Safe_Release(pLodingBack);
		MSG_BOX("pLodingBack Create Failed");
		return nullptr;
	}

	return pLodingBack;
}

void CLoadingCircle::Update_Pos(_float LoadingPercent)
{
	_float fScaleY = m_pTransformCom->Get_World()->m[1][1];
	_float fTargetY = LoadingPercent * fScaleY;
	_float flerpY = m_vPos.y + (fTargetY - m_vPos.y) * 0.1;
	m_pTransformCom->Set_Pos(m_vPos.x, flerpY, m_vPos.z);
}

void CLoadingCircle::Free()
{
	CUi::Free();
}