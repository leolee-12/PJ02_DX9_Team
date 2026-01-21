#include "pch.h"
#include "CCookingSelectSlot.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CCookingSelectSlot::CCookingSelectSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CCookingSelectSlot::~CCookingSelectSlot()
{
}

HRESULT CCookingSelectSlot::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(232 * m_fScale, 232 * m_fScale, 0.f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	Ready_PixelShader();

	return S_OK;
}

HRESULT CCookingSelectSlot::Ready_Material()
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

_int CCookingSelectSlot::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CCookingSelectSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CCookingSelectSlot::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	D3DXCOLOR tColor = { 0.5f, 0.5f, 0.5f, 1.f };
	m_pGraphicDev->SetPixelShader(m_pPixelShader);
	m_pGraphicDev->SetPixelShaderConstantF(0, (float*)tColor, 1);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetPixelShader(NULL);
}

void CCookingSelectSlot::OnCollision(CGameObject* pObject)
{

}

HRESULT CCookingSelectSlot::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingSelectSlot"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

HRESULT CCookingSelectSlot::Ready_PixelShader()
{
	LPD3DXBUFFER pCode = NULL;
	LPD3DXBUFFER pError = NULL;

	// HLSL 파일 컴파일 
	HRESULT hr = D3DXCompileShaderFromFile(
		L"../Shader/TextureColorBlend.hlsl", // 파일명 
		NULL, // 매크로 
		NULL, // include 
		"PS_TintPixel", // 엔트리 포인트 
		"ps_2_0", // 셰이더 모델 
		0, // 플래그 
		&pCode,
		&pError,
		NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(NULL,
				(char*)pError->GetBufferPointer(),
				"Shader Error",
				MB_OK);
			pError->Release();
		}
		return E_FAIL;
	} // 픽셀 셰이더 생성 

	if (pCode) {
		m_pGraphicDev->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &m_pPixelShader);
		pCode->Release();
	}

	if (pError) {
		pError->Release();
	}

	return S_OK;
}


CCookingSelectSlot* CCookingSelectSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fScale)
{
	CCookingSelectSlot* pCookingSlot = new CCookingSelectSlot(pGraphicDev);

	pCookingSlot->m_vPos = vPos;
	pCookingSlot->m_fScale = fScale;


	if (FAILED(pCookingSlot->Ready_GameObject()))
	{
		Safe_Release(pCookingSlot);
		MSG_BOX("pCCookingSlot Create Failed");
		return nullptr;
	}

	return pCookingSlot;
}

void CCookingSelectSlot::Free()
{
	CUi::Free();
}
