#include "pch.h"
#include "CDungeonIcon.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

CDungeonIcon::CDungeonIcon(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_bRender(true)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
	ZeroMemory(&m_vColor, sizeof(_vec3));
}

CDungeonIcon::CDungeonIcon(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
	: CUi(pGraphicDev, pMessageChannel), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_bRender(true)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
	ZeroMemory(&m_vColor, sizeof(_vec3));
}

CDungeonIcon::~CDungeonIcon()
{
}

HRESULT CDungeonIcon::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(300.f, 300.f, 1.f);
	//m_pTransformCom->Set_Pos(_float(-WINCX / 2) + 300.f, _float(-WINCY / 2) + 80.f, 0.f);
	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);

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

	m_vColor = { 1.f, 1.f, 1.f };

	return S_OK;
}

_int CDungeonIcon::Update_GameObject(const _float& fTimeDelta)
{
	if (CDInputMgr::GetInstance()->Key_Down(DIK_T))
	{
		m_vColor.y != 0.f ? m_vColor.y = 0.f : m_vColor.y = 1.f;
		m_vColor.z != 0.f ? m_vColor.z = 0.f : m_vColor.z = 1.f;
	}
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (m_bRender) { CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this); }

	return iExit;
}

void CDungeonIcon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CDungeonIcon::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(m_vColor.x, m_vColor.y, m_vColor.z, 1.f);
	tMtrl.Specular = D3DXCOLOR(m_vColor.x, m_vColor.y, m_vColor.z, 1.f);
	tMtrl.Ambient = D3DXCOLOR(m_vColor.x, m_vColor.y, m_vColor.z, 1.f);

	tMtrl.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 50.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	m_pTextureCom_BG->Set_Texture();
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pTextureCom_MG->Set_Texture();
	m_pBufferCom->Render_Buffer();

	_matrix matTmp;
	D3DXMatrixIdentity(&matTmp);
	matTmp._11 = 1.f / 8.f;
	matTmp._22 = 1.f / 8.f;

	matTmp._31 = 0.f;
	matTmp._32 = 7.f / 8.f;
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &matTmp);

	m_pTextureCom_Icon->Set_Texture();
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CDungeonIcon::OnCollision(CGameObject* pObject)
{

}

HRESULT CDungeonIcon::Add_Component()
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

	pComponent = m_pTextureCom_Icon = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DungeonIcon"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_DungeonIcon", pComponent });

	pComponent = m_pTextureCom_MG = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DungeonMG"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_DungeonMG", pComponent });

	pComponent = m_pTextureCom_BG = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DungeonBG"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_DungeonBG", pComponent });

	return S_OK;
}

CDungeonIcon* CDungeonIcon::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CDungeonIcon* pLodingBack = new CDungeonIcon(pGraphicDev, pMessageChannel);

	if (FAILED(pLodingBack->Ready_GameObject()))
	{
		Safe_Release(pLodingBack);
		MSG_BOX("pLodingBack Create Failed");
		return nullptr;
	}

	return pLodingBack;
}

void CDungeonIcon::Free()
{
	CUi::Free();
}