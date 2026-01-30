#include "pch.h"
#include "CPlayerTarotCard.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CPlayerTarotCard::CPlayerTarotCard(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CPlayerTarotCard::~CPlayerTarotCard()
{
}

HRESULT CPlayerTarotCard::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(100.0f * 0.5f, 100.0f* 0.5f, 0.f);
	m_pTransformCom->Set_Pos(-460.f, 250.f, 0.1f);

	m_bRender = false;
	Ready_Event();
	return S_OK;
}

HRESULT CPlayerTarotCard::Ready_Material()
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

_int CPlayerTarotCard::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CPlayerTarotCard::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CPlayerTarotCard::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pTextureCom->Set_Texture();


	m_pBufferCom->Render_Buffer();
}

void CPlayerTarotCard::OnCollision(CGameObject* pObject)
{

}

HRESULT CPlayerTarotCard::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlayerTarotCard"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CPlayerTarotCard* CPlayerTarotCard::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CPlayerTarotCard* pGaugeCover = new CPlayerTarotCard(pGraphicDev);

	pGaugeCover->m_pMessageChannel = pMessageChannel;
	pGaugeCover->m_pMessageChannel->AddRef();

	if (FAILED(pGaugeCover->Ready_GameObject()))
	{
		Safe_Release(pGaugeCover);
		MSG_BOX("pGaugeCover Create Failed");
		return nullptr;
	}

	return pGaugeCover;
}

void CPlayerTarotCard::Free()
{
	CUi::Free();
}

void CPlayerTarotCard::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Tarot.Selected", m_pMessageChannel->Subscribe(L"Tarot.Selected", [this](const IMessageChannel::EVENT& Event)
		{
			m_bRender = true;
		}
	) });
}
