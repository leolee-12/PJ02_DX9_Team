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

	m_pNameFont = CFontUIOrtho::Create(m_pGraphicDev);

	m_pNameFont->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pNameFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	m_pNameFont->Set_Pos(_vec2(m_vWorldPos.x, m_vWorldPos.y));
	m_pNameFont->Set_Scale(_vec2(59.f * 2.f, 123.f * 0.5f));
	m_pNameFont->Set_Font(L"Font_Default24");
	m_pNameFont->Set_Text(m_szName);
	m_pNameFont->Active();
	m_bRender = true;

	m_pTransformCom->Set_Scale(293.0f * m_fScale, 94.0f * m_fScale, 1.0f);
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	m_vScreenPos = _vec2(WINCX / 2 + m_vWorldPos.x, WINCY / 2 - m_vWorldPos.y);
	m_vHitHalfScale = _vec2((293.0f * m_fScale) / 2, (94.0f * m_fScale) / 2);

	m_bOnClick = false;
	return S_OK;
}

_int CInvenBtn::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	Check_CusorColl();

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	m_pNameFont->Set_Pos(_vec2(m_vWorldPos.x, m_vWorldPos.y));
	m_pNameFont->Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return iExit;
}

void CInvenBtn::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	m_vWorldPos = m_vParentPos + m_vLocalPos;
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	m_vScreenPos = _vec2(WINCX / 2 + m_vWorldPos.x, WINCY / 2 - m_vWorldPos.y);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pNameFont->LateUpdate_GameObject(fTimeDelta);
	Compute_ViewDepth_Ortho(&m_vWorldPos);
}

void CInvenBtn::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iPage);

	m_pBufferCom->Render_Buffer();
	m_pNameFont->Render_GameObject();
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
			m_bOnClick = true;
		}
		m_iPage = 1;
		m_pNameFont->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
	else {
		m_iPage = 0;
		m_pNameFont->Set_FontColor(D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
	}
}

CInvenBtn* CInvenBtn::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale)
{
	CInvenBtn* pCInvenBtn = new CInvenBtn(pGraphicDev);
	pCInvenBtn->m_vLocalPos = _vLocalPos;
	pCInvenBtn->m_vParentPos = _vParentPos;
	pCInvenBtn->m_fScale = _fScale;
	pCInvenBtn->m_vWorldPos = _vLocalPos + _vParentPos;
	if (FAILED(pCInvenBtn->Ready_GameObject()))
	{
		Safe_Release(pCInvenBtn);
		MSG_BOX("pCInvenBtn Create Failed");
		return nullptr;
	}
	return pCInvenBtn;
}

void CInvenBtn::Set_Tex(wstring _szName)
{
	m_pNameFont->Set_Text(_szName);
}

void CInvenBtn::Free()
{
	Safe_Release(m_pNameFont);
	CUi::Free();
}
