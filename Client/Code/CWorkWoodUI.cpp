#include "pch.h"
#include "CWorkWoodUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CWorkWoodUI::CWorkWoodUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CWorkWoodUI::~CWorkWoodUI()
{
}

HRESULT CWorkWoodUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(106.0f * m_fScale, 90.0f * m_fScale , 1.0f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
	m_vScreenPos = _vec2(WINCX / 2 + m_vPos.x , WINCY / 2 - m_vPos.y);
	m_vHitHalfScale = _vec2((106.0f * m_fScale) / 2, (90.0f * m_fScale) / 2);	

	return S_OK;
}

_int CWorkWoodUI::Update_GameObject(const _float& fTimeDelta)
{
	Check_CusorColl();

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CWorkWoodUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CWorkWoodUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CWorkWoodUI::OnCollision(CGameObject* pObject)
{

}

HRESULT CWorkWoodUI::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_NPCCommaderUIWood"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


void CWorkWoodUI::Check_CusorColl()
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
			IMessageChannel::EVENT WorkWoodEvent;
			WorkWoodEvent.strType = L"CWorkWood.Selected";
			//WorkWoodEvent.hmapData[L"WorkType"];
			m_pMessageChannel->Publish(WorkWoodEvent);
		}
	}

}


CWorkWoodUI* CWorkWoodUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, _vec3 _vPos, float _fScale)
{
	CWorkWoodUI* pCWorkWoodUI = new CWorkWoodUI(pGraphicDev);
	pCWorkWoodUI->m_pMessageChannel = pMessageChannel;
	pCWorkWoodUI->m_pMessageChannel->AddRef();
	pCWorkWoodUI->m_vPos = _vPos;
	pCWorkWoodUI->m_fScale = _fScale;


	if (FAILED(pCWorkWoodUI->Ready_GameObject()))
	{
		Safe_Release(pCWorkWoodUI);
		MSG_BOX("pCWorkWoodUI Create Failed");
		return nullptr;
	}
	return pCWorkWoodUI;
}

void CWorkWoodUI::Free()
{
	CUi::Free();
}
