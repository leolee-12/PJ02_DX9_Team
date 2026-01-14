#include "pch.h"
#include "CCookingGauge.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CCookingGauge::CCookingGauge(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CCookingGauge::~CCookingGauge()
{
}

HRESULT CCookingGauge::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(598 * 0.4f* 0.3f, 76 * 0.4f, 0.f);
	m_pTransformCom->Set_Pos(0, WINCY / 5, 0.2f);

	fxRange = 50.0f;
	return S_OK;
}

HRESULT CCookingGauge::Ready_Material()
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

_int CCookingGauge::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CCookingGauge::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CCookingGauge::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pTextureCom->Set_Texture();


	m_pBufferCom->Render_Buffer();
}

void CCookingGauge::OnCollision(CGameObject* pObject)
{

}

HRESULT CCookingGauge::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingBarGreen"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CCookingGauge::Set_RandomPosX()
{

	float fRandomX = Engine::Get_Rand_Float(-fxRange, fxRange);

	m_pTransformCom->Set_Pos(fRandomX, WINCY / 5, 0.2f);
}

void CCookingGauge::Get_GaugeRange(float& fLeft, float& fRight)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);


	// 0.3f임시 값 상수로 넣어둔거 나중에 수정해야함
	float  xHarlf = (598.f * 0.4f *0.3f * 0.5f) * 0.5f;

	fLeft  = vPos.x - xHarlf;
	fRight = vPos.x + xHarlf;
}



CCookingGauge* CCookingGauge::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCookingGauge* pGaugeCover = new CCookingGauge(pGraphicDev);

	if (FAILED(pGaugeCover->Ready_GameObject()))
	{
		Safe_Release(pGaugeCover);
		MSG_BOX("pCookingGauge Create Failed");
		return nullptr;
	}

	return pGaugeCover;
}

void CCookingGauge::Free()
{
	CUi::Free();
}