#include "pch.h"
#include "CGaugeStar.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CGaugeStar::CGaugeStar(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
	, m_bRender(false)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CGaugeStar::~CGaugeStar()
{
}

HRESULT CGaugeStar::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(30.f * 2.f, 28.f * 2.f, 0.f);
	m_pTransformCom->Set_Pos(-345.f - 240.f, 243.f + 60.f, 0.11f);

	return S_OK;
}

HRESULT CGaugeStar::Ready_Material()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

_int CGaugeStar::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (m_bRender) {
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CGaugeStar::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CGaugeStar::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	// 텍스처 알파 × 전체 알파만 계산
	DWORD oldAlphaOp;
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &oldAlphaOp);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	// 전체 알파값 지정
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(128, 255, 255, 255));

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, oldAlphaOp);
}

void CGaugeStar::OnCollision(CGameObject* pObject)
{

}

HRESULT CGaugeStar::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GaugeStar"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CGaugeStar* CGaugeStar::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CGaugeStar* pGaugeCover = new CGaugeStar(pGraphicDev);

	if (FAILED(pGaugeCover->Ready_GameObject()))
	{
		Safe_Release(pGaugeCover);
		MSG_BOX("pGaugeCover Create Failed");
		return nullptr;
	}

	return pGaugeCover;
}

void CGaugeStar::Free()
{
	CUi::Free();
}