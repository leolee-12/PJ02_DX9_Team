#include "CCursor.h"
#include "CProtoMgr.h"

CCursor::CCursor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
{
}

CCursor::~CCursor()
{
}

HRESULT	CCursor::Ready_GameObject()
{

}

_int CCursor::Update_GameObject(const _float& fTimeDelta)
{

}

void CCursor::Render_GameObject()
{

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
