#include "pch.h"
#include "CKBBoardBack.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CKBBoardBack::CKBBoardBack(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_bSelect(false)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CKBBoardBack::~CKBBoardBack()
{
}

HRESULT CKBBoardBack::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(0.f, -185.f, 0.255f);
	m_pTransformCom->Set_Scale(110.f, 210.f, 1.f);

	return S_OK;
}

_int CKBBoardBack::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (m_bSelect) {
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CKBBoardBack::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CKBBoardBack::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	D3DMATERIAL9 tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CKBBoardBack::OnCollision(CGameObject* pObject)
{
}

HRESULT CKBBoardBack::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTex
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

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_KBBoardSlot"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void		CKBBoardBack::Move_Right()
{
	m_pTransformCom->Set_Pos(m_vPos.x + 115.f, m_vPos.y, m_vPos.z);
}
void		CKBBoardBack::Move_Left()
{
	m_pTransformCom->Set_Pos(m_vPos.x - 115.f, m_vPos.y, m_vPos.z);
}
void 		CKBBoardBack::Move_Center()
{
	m_pTransformCom->Set_Pos(0.f, m_vPos.y, m_vPos.z);
}

CKBBoardBack* CKBBoardBack::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKBBoardBack* pSlot = new CKBBoardBack(pGraphicDev);

	if (FAILED(pSlot->Ready_GameObject()))
	{
		Safe_Release(pSlot);
		MSG_BOX("CKBBoardBack Create Failed");
		return nullptr;
	}

	return pSlot;
}

void CKBBoardBack::Free()
{
	CUi::Free();
}
