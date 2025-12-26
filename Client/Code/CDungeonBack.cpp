#include "pch.h"
#include "CDungeonBack.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CDungeonBack::CDungeonBack(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_bRender(false)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CDungeonBack::CDungeonBack(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pChannel)
	: CUi(pGraphicDev, pChannel), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_bRender(false)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CDungeonBack::~CDungeonBack()
{
}

HRESULT CDungeonBack::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale((_float)WINCX, (_float)WINCY, 1.f);
	m_pTransformCom->Set_Pos(0.f, 0.f, 0.5f);

	m_hmapSubHandles.insert({ L"Render", m_pMessageChannel->Subscribe(
		L"Select", [this](const IMessageChannel::EVENT&)
		{
			if (m_bRender)
			{
				m_bRender = false;
				return;
			}
			m_bRender = true;
		}
	) });
	

	return S_OK;
}

_int CDungeonBack::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (m_bRender) { CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this); }

	return iExit;
}

void CDungeonBack::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CDungeonBack::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetTexture(0, NULL);

	m_pBufferCom->Render_Buffer();
}

void CDungeonBack::OnCollision(CGameObject* pObject)
{

}

HRESULT CDungeonBack::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcCol*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcCol"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}



CDungeonBack* CDungeonBack::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pChannel)
{
	CDungeonBack* pLodingBack = new CDungeonBack(pGraphicDev, pChannel);

	if (FAILED(pLodingBack->Ready_GameObject()))
	{
		Safe_Release(pLodingBack);
		MSG_BOX("pLodingBack Create Failed");
		return nullptr;
	}

	return pLodingBack;
}

void CDungeonBack::Free()
{
	CUi::Free();
}