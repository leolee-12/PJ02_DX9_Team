#include "pch.h"
#include "CTitleTab.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CTitleTab::CTitleTab(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CTitleTab::~CTitleTab()
{
}

HRESULT CTitleTab::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale((607.f * 0.5f), (123.f * 0.5f), 1.f);
	m_pTransformCom->Set_Pos((_float(-WINCX) * 0.5f) + 220.f, _float(WINCY / 2) - 340.f, 0.f);

	return S_OK;
}

HRESULT CTitleTab::Ready_Material()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	tMtrl.Power = 50.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

_int CTitleTab::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CTitleTab::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CTitleTab::Render_GameObject()
{
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	/*if (FAILED(Ready_Material()))
		return;*/

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CTitleTab::OnCollision(CGameObject* pObject)
{

}

HRESULT CTitleTab::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MainTabTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CTitleTab* CTitleTab::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTitleTab* pTitleLogo = new CTitleTab(pGraphicDev);

	if (FAILED(pTitleLogo->Ready_GameObject()))
	{
		Safe_Release(pTitleLogo);
		MSG_BOX("pTitleLogo Create Failed");
		return nullptr;
	}

	return pTitleLogo;
}

void CTitleTab::Move_Down()
{
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y - 100.f, m_vPos.z);
}

void CTitleTab::Move_Up()
{
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y + 100.f, m_vPos.z);
}

void CTitleTab::Free()
{
	CUi::Free();
}