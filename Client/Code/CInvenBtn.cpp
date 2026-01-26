#include "pch.h"
#include "CInvenBtn.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CFontUIOrtho.h"

CInvenBtn::CInvenBtn(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CInvenBtn::~CInvenBtn()
{
}

HRESULT CInvenBtn::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = m_pName = CFontUIOrtho::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	m_pName->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pName->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pName->Set_Pos(_vec2(0.0f, 230.0f));
	m_pName->Set_Scale(_vec2(59.f * 2.f, 123.f * 0.5f));
	m_pName->Set_Font(L"Font_Default30_Heavy");
	m_pName->Set_Text(L"소지품");
	m_pName->Active();

	m_bRender = true;
	return S_OK;
}

_int CInvenBtn::Update_GameObject(const _float& fTimeDelta)
{
	Check_CusorColl();

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	m_pName->Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return iExit;
}

void CInvenBtn::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pName->LateUpdate_GameObject(fTimeDelta);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CInvenBtn::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CInvenBtn::OnCollision(CGameObject* pObject)
{

}

HRESULT CInvenBtn::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InventoryButton"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


void CInvenBtn::Check_CusorColl()
{
	//if (!m_bRender) { return; }

	POINT pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	RECT tRc = {
		_long(m_vScreenPos.x - m_vHitHalfScale.x),
		_long(m_vScreenPos.y - m_vHitHalfScale.y),
		_long(m_vScreenPos.x + m_vHitHalfScale.x),
		_long(m_vScreenPos.y + m_vHitHalfScale.y)
	};


	if (PtInRect(&tRc, pt))
	{

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			//IMessageChannel::EVENT WorkWoodEvent;
			//WorkWoodEvent.strType = L"CWorkWood.Selected";
			//m_pMessageChannel->Publish(WorkWoodEvent);
		}
		m_pTransformCom->Set_Scale(106.0f * m_fScale * 1.3f, 90.0f * m_fScale * 1.3f, 1.0f);
	}
	else {
		m_pTransformCom->Set_Scale(106.0f * m_fScale, 90.0f * m_fScale, 1.0f);
	}
}



CInvenBtn* CInvenBtn::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale)
{
	CInvenBtn* pCInvenBtn = new CInvenBtn(pGraphicDev);
	pCInvenBtn->m_vPos = _vPos;
	pCInvenBtn->m_fScale = _fScale;


	if (FAILED(pCInvenBtn->Ready_GameObject()))
	{
		Safe_Release(pCInvenBtn);
		MSG_BOX("pCInvenBtn Create Failed");
		return nullptr;
	}
	return pCInvenBtn;
}

void CInvenBtn::Free()
{
	CUi::Free();
}
