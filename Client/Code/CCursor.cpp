#include "pch.h"
#include "CCursor.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CCursor::CCursor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
}

CCursor::~CCursor()
{
}

HRESULT	CCursor::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);
	m_pTransformCom->Set_Scale(32.f, 32.f, 0.f);
	m_fDepth = -0.1f;

	return S_OK;
}

_int CCursor::Update_GameObject(const _float& fTimeDelta)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	// 스크린 좌표 → DX 좌표
	_float fX = pt.x - _float(WINCX) * 0.5f;
	_float fY = -pt.y + _float(WINCY) * 0.5f;
	_float fHalfScale = 16.f;

	m_pTransformCom->Set_Pos(fX + fHalfScale, fY - fHalfScale, 0.f);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CCursor::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	_matrix matOldView, matOldProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);

	_matrix matView, matProj;
	D3DXMatrixIdentity(&matView);
	D3DXMatrixOrthoLH(&matProj, (_float)WINCX, (_float)WINCY, 0.f, 1.f);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);
	
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

HRESULT CCursor::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MouseCursor"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


	return S_OK;
}

CCursor* CCursor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCursor* pCursor = new CCursor(pGraphicDev);

	if (FAILED(pCursor->Ready_GameObject()))
	{
		Safe_Release(pCursor);
		MSG_BOX("pCursor Create Failed");
		return nullptr;
	}

	return pCursor;
}

void CCursor::Free()
{
	CUi::Free();
}
