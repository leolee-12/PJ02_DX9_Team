#include "pch.h"
#include "CCookingInfoFaith.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CCookingInfoFaith::CCookingInfoFaith(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CCookingInfoFaith::~CCookingInfoFaith()
{
}

HRESULT CCookingInfoFaith::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(73 * m_fScale, 76 * m_fScale, 0.1f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, 0.1f);

	return S_OK;
}

HRESULT CCookingInfoFaith::Ready_Material()
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

_int CCookingInfoFaith::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CCookingInfoFaith::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CCookingInfoFaith::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pTextureCom->Set_Texture();


	m_pBufferCom->Render_Buffer();
}

void CCookingInfoFaith::OnCollision(CGameObject* pObject)
{

}

HRESULT CCookingInfoFaith::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingCardInfoFaithTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


CCookingInfoFaith* CCookingInfoFaith::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale)
{
	CCookingInfoFaith* pCookingInfoFaith = new CCookingInfoFaith(pGraphicDev);

	pCookingInfoFaith->m_vPos = _vPos;
	pCookingInfoFaith->m_fScale = _fScale;


	if (FAILED(pCookingInfoFaith->Ready_GameObject()))
	{
		Safe_Release(pCookingInfoFaith);
		MSG_BOX("pCookingInfoFaith Create Failed");
		return nullptr;
	}
	return pCookingInfoFaith;
}

void CCookingInfoFaith::Free()
{
	CUi::Free();
}
