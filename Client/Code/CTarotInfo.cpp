#include "pch.h"
#include "CTarotInfo.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CTarotInfo::CTarotInfo(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
	, m_bActive(false)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CTarotInfo::~CTarotInfo()
{
}

HRESULT CTarotInfo::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	m_pTransformCom->Set_Scale(300.f, 275.f, 1.f);
	m_pTransformCom->Set_Pos(m_vCenterPos.x, m_vCenterPos.y, 0.11f);
	return S_OK;
}

_int CTarotInfo::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bActive)
	{
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CTarotInfo::LateUpdate_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CTarotInfo::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(_uint(m_iType));

	m_pBufferCom->Render_Buffer();
}

void CTarotInfo::OnCollision(CGameObject* pObject)
{

}

HRESULT CTarotInfo::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTexUI"));

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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TarotInfo"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CTarotInfo* CTarotInfo::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 _vPos, _uint iType)
{
	CTarotInfo* pTarotInfo = new CTarotInfo(pGraphicDev);

	pTarotInfo->m_vCenterPos = _vPos;
	pTarotInfo->m_iType = iType;

	if (FAILED(pTarotInfo->Ready_GameObject()))
	{
		Safe_Release(pTarotInfo);
		MSG_BOX("pTarotInfo Create Failed");
		return nullptr;
	}

	return pTarotInfo;
}

void CTarotInfo::Free()
{
	CUi::Free();
}
