#include "pch.h"
#include "CCookingSelectButton.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"

CCookingSelectButton::CCookingSelectButton(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CCookingSelectButton::~CCookingSelectButton()
{
}

HRESULT CCookingSelectButton::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(413.0f * m_fScale, 100.0f * m_fScale, 0.f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	return S_OK;
}

HRESULT CCookingSelectButton::Ready_Material()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

_int CCookingSelectButton::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CCookingSelectButton::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CCookingSelectButton::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iPage);

	m_pBufferCom->Render_Buffer();

	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	wchar_t szFoodName[32];

	swprintf_s(szFoodName, L"요리");
	RECT rc = { 310, 625, 380, 680 };
	CFontMgr::GetInstance()->Render_Font(L"Font_Default24", szFoodName, rc, FontColor, DT_CENTER | DT_TOP);
}

void CCookingSelectButton::OnCollision(CGameObject* pObject)
{

}
HRESULT CCookingSelectButton::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingSelectButton"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CCookingSelectButton* CCookingSelectButton::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iPage, _vec3 vPos, _float fScale)
{
	CCookingSelectButton* pCookingSelectButton = new CCookingSelectButton(pGraphicDev);

	pCookingSelectButton->m_vPos = vPos;
	pCookingSelectButton->m_fScale = fScale;
	pCookingSelectButton->m_iPage = iPage;

	if (FAILED(pCookingSelectButton->Ready_GameObject()))
	{
		Safe_Release(pCookingSelectButton);
		MSG_BOX("pCookingSelectButton Create Failed");
		return nullptr;
	}

	return pCookingSelectButton;
}

void CCookingSelectButton::Free()
{
	CUi::Free();
}
