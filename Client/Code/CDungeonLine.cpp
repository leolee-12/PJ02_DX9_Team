#include "pch.h"
#include "CDungeonLine.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

CDungeonLine::CDungeonLine(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_fFrame(0.f)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
	ZeroMemory(&m_tMtrl, sizeof(_vec3));
	ZeroMemory(&m_vColor, sizeof(_vec3));
}

CDungeonLine::~CDungeonLine()
{
}

HRESULT CDungeonLine::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(50.f, 250.f, 1.f);

	m_hmapSubHandles.insert({ L"Render", m_pMessageChannel->Subscribe(
		L"Select", [this](const IMessageChannel::EVENT& event)
		{
			if (m_bRender)
			{
				m_bRender = false;
				return;
			}
			m_bRender = true;
		}
	) });

	m_hmapSubHandles.insert({ L"Animation", m_pMessageChannel->Subscribe(
		L"Choose", [this](const IMessageChannel::EVENT& event)
		{
			auto iter = event.hmapData.find(L"Look_Stage");
			if (iter == event.hmapData.end()) { return; }

			if (any_cast<DLINESTATE>(iter->second) == m_eState)
			{
				m_bAnim = true;
			}
		}
	) });

	m_hmapSubHandles.insert({ L"Change_Line", m_pMessageChannel->Subscribe(
		L"Enter", [this](const IMessageChannel::EVENT& event)
		{
			auto iter = event.hmapData.find(L"Look_Stage");
			if (iter == event.hmapData.end()) { return; }

			if (any_cast<DLINESTATE>(iter->second) == m_eState)
			{
				m_fFrame = 1;
			}
		}
	) });

	switch (m_eState)
	{
	case DL_1:
		m_pTransformCom->Set_Pos(0.f, -100.f, 0.25f);
		break;
	case DL_2:
		m_pTransformCom->Set_Pos(0.f, 100.f, 0.25f);
		break;
	}

	m_vColor = { 1.f, 1.f, 1.f };

	D3DXMatrixIdentity(&m_matUV);


	return S_OK;
}

_int CDungeonLine::Update_GameObject(const _float& fTimeDelta)
{
	if (CDInputMgr::GetInstance()->Key_Down(DIK_Y))
	{
		m_vColor.y != 0.f ? m_vColor.y = 0.f : m_vColor.y = 1.f;
		m_vColor.z != 0.f ? m_vColor.z = 0.f : m_vColor.z = 1.f;
	}
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (m_bRender) { CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this); }

	return iExit;
}

void CDungeonLine::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);

	m_tMtrl.Diffuse = D3DXCOLOR(m_vColor.x, m_vColor.y, m_vColor.z, 1.f);
	m_tMtrl.Specular = D3DXCOLOR(m_vColor.x, m_vColor.y, m_vColor.z, 1.f);
	m_tMtrl.Ambient = D3DXCOLOR(m_vColor.x, m_vColor.y, m_vColor.z, 1.f);

	m_tMtrl.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	m_tMtrl.Power = 50.f;
}

void CDungeonLine::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetMaterial(&m_tMtrl);

	if (m_bAnim){ m_matUV._32 += 0.003; }

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matUV);

	m_pTextureCom->Set_Texture(m_fFrame);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CDungeonLine::OnCollision(CGameObject* pObject)
{

}

HRESULT CDungeonLine::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DungeonLine"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CDungeonLine* CDungeonLine::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pChannel, DLINESTATE eState)
{
	CDungeonLine* pDungeonLine = new CDungeonLine(pGraphicDev);

	pDungeonLine->m_pMessageChannel = pChannel;
	pDungeonLine->m_pMessageChannel->AddRef();
	pDungeonLine->m_eState = eState;

	if (FAILED(pDungeonLine->Ready_GameObject()))
	{
		Safe_Release(pDungeonLine);
		MSG_BOX("pDungeonLine Create Failed");
		return nullptr;
	}

	return pDungeonLine;
}

void CDungeonLine::Free()
{
	CUi::Free();
}