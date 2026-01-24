#include "pch.h"
#include "CRestUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CRestUI::CRestUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CRestUI::~CRestUI()
{
}

HRESULT CRestUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(103.0f * m_fScale, 87.0f * m_fScale, 1.0f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	m_vScreenPos = _vec2(WINCX / 2 + m_vPos.x, WINCY / 2 - m_vPos.y);
	m_vHitHalfScale = _vec2((106.0f * m_fScale) / 2, (90.0f * m_fScale) / 2);

	return S_OK;
}

_int CRestUI::Update_GameObject(const _float& fTimeDelta)
{
	Check_CusorColl();
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CRestUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CRestUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CRestUI::OnCollision(CGameObject* pObject)
{

}

HRESULT CRestUI::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_NPCCommaderUIRest"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CRestUI::Check_CusorColl()
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
			IMessageChannel::EVENT WorkRestEvent;
			WorkRestEvent.strType = L"CWorkRest.Selected";
			/*TarotEvent.hmapData[L"TarotType"];*/
			m_pMessageChannel->Publish(WorkRestEvent);
		}
	}

}




CRestUI* CRestUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, _vec3 _vPos, float _fScale)
{
	CRestUI* pCRestUI = new CRestUI(pGraphicDev);

	pCRestUI->m_pMessageChannel = pMessageChannel;
	pCRestUI->m_pMessageChannel->AddRef();

	pCRestUI->m_vPos = _vPos;
	pCRestUI->m_fScale = _fScale;


	if (FAILED(pCRestUI->Ready_GameObject()))
	{
		Safe_Release(pCRestUI);
		MSG_BOX("pCRestUI Create Failed");
		return nullptr;
	}
	return pCRestUI;
}

void CRestUI::Free()
{
	CUi::Free();
}
