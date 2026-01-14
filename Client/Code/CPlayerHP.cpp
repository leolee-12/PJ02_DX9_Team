#include "pch.h"
#include "CPlayerHP.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CPlayerHP::CPlayerHP(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
	, m_iPlayerHp(0), m_iPlayerMaxHp(0)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CPlayerHP::CPlayerHP(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pChannel)
	: CUi(pGraphicDev, pChannel), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
	, m_iPlayerHp(0), m_iPlayerMaxHp(0)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CPlayerHP::~CPlayerHP()
{
}

HRESULT CPlayerHP::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale((98.f * 0.25f) , (94.f * 0.25f), 1.f);
	m_pTransformCom->Set_Pos(-450.f, 280.f, 0.8f);

	m_iPlayerHp = 7;
	m_iPlayerMaxHp = 8;

	return S_OK;
}

_int CPlayerHP::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (m_bRender) { CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this); }

	return iExit;
}

void CPlayerHP::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CPlayerHP::Render_GameObject()
{
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	_int iUICell = m_iPlayerMaxHp / 2;

	for (_int i = 0; i < iUICell; ++i)
	{
		_matrix matTmp = *(m_pTransformCom->Get_World());
		_float fMoveX = 25.f;
		fMoveX *= i;
		matTmp._41 += fMoveX;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matTmp);

		if (m_iPlayerHp > 2 + (i * 2)) {
			m_pTextureCom_Full->Set_Texture(2);
		}
		else {
			_int iCellHP = m_iPlayerHp - (i * 2);
			if (iCellHP < 0)
				iCellHP = 0;

			m_pTextureCom_Full->Set_Texture(iCellHP);
		}

		m_pBufferCom->Render_Buffer();
	}

}

void CPlayerHP::OnCollision(CGameObject* pObject)
{

}

HRESULT CPlayerHP::Add_Component()
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


	pComponent = m_pTextureCom_Full = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlayerHP"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_PlayerHP", pComponent });

	/*pComponent = m_pTextureCom_Half = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlayerHalfHP"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_PlayerHalfHP", pComponent });*/

	return S_OK;
}



CPlayerHP* CPlayerHP::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pChannel)
{
	CPlayerHP* pLodingBack = new CPlayerHP(pGraphicDev, pChannel);

	if (FAILED(pLodingBack->Ready_GameObject()))
	{
		Safe_Release(pLodingBack);
		MSG_BOX("pLodingBack Create Failed");
		return nullptr;
	}

	return pLodingBack;
}

void CPlayerHP::Free()
{
	CUi::Free();
}