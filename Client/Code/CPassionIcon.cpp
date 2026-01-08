#include "pch.h"
#include "CPassionIcon.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CPassionIcon::CPassionIcon(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
	, m_eCurState(Gauge::GS_END), m_ePreState(Gauge::GS_END)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CPassionIcon::~CPassionIcon()
{
}

HRESULT CPassionIcon::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPassionIcon::Ready_Material()
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

void CPassionIcon::Update_State()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Gauge::GS_PASSION:
			m_iIndex = 0;
			m_pTransformCom->Set_Scale(33.f * 0.5f, 42.f * 0.5f, 0.f);
			m_pTransformCom->Set_Pos(-330.f - 245.f, 243.f + 60.f, 0.1f);
			break;
		case Gauge::GS_FAITH:
			m_iIndex = 1;
			m_pTransformCom->Set_Scale(103.f * 0.25f, 87.f * 0.25f, 0.f);
			m_pTransformCom->Set_Pos(-345.f - 240.f, 240.f + 60.f, 0.1f);
			break;
		default:
			break;
		}
		m_ePreState = m_eCurState;
	}
}

_int CPassionIcon::Update_GameObject(const _float& fTimeDelta)
{
	Update_State();
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CPassionIcon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CPassionIcon::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pTextureCom->Set_Texture(m_iIndex);


	m_pBufferCom->Render_Buffer();
}

void CPassionIcon::OnCollision(CGameObject* pObject)
{

}

HRESULT CPassionIcon::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PassionIcon"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CPassionIcon* CPassionIcon::Create(LPDIRECT3DDEVICE9 pGraphicDev, Gauge::GAUGESTATE eState)
{
	CPassionIcon* pPassionIcon = new CPassionIcon(pGraphicDev);

	if (FAILED(pPassionIcon->Ready_GameObject()))
	{
		Safe_Release(pPassionIcon);
		MSG_BOX("pPassionIcon Create Failed");
		return nullptr;
	}

	return pPassionIcon;
}

void CPassionIcon::Free()
{
	CUi::Free();
}