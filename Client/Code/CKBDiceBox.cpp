#include "pch.h"
#include "CKBDiceBox.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CKBDiceBox::CKBDiceBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CKBDiceBox::~CKBDiceBox()
{
}

HRESULT CKBDiceBox::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(802.f * 0.3f, 546.f * 0.28f, 1.f);

	switch (m_iOwner)
	{
	case 0: // 플레이어
		m_pTransformCom->Set_Pos((_float(-WINCX) * 0.5f) + 220.f, _float(-WINCY / 2) + 140.f, 0.5f);
		m_tColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		break;
	case 1: // NPC
		m_pTransformCom->Set_Pos((_float(WINCX) * 0.5f) - 220.f, _float(WINCY / 2) - 140.f, 0.5f);
		m_tColor = D3DXCOLOR(0.7f, 0.9f, 1.f, 1.f);
		break;
	}

	return S_OK;
}

_int CKBDiceBox::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CKBDiceBox::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CKBDiceBox::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	D3DMATERIAL9 tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Emissive = m_tColor;
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CKBDiceBox::OnCollision(CGameObject* pObject)
{

}

HRESULT CKBDiceBox::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_KBDiceBox"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CKBDiceBox* CKBDiceBox::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iOwner)
{
	CKBDiceBox* pDiceBox = new CKBDiceBox(pGraphicDev);
	pDiceBox->m_iOwner = iOwner;

	if (FAILED(pDiceBox->Ready_GameObject()))
	{
		Safe_Release(pDiceBox);
		MSG_BOX("pDiceBox Create Failed");
		return nullptr;
	}

	return pDiceBox;
}

void CKBDiceBox::Free()
{
	CUi::Free();
}