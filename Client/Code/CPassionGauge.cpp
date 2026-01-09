#include "pch.h"
#include "CPassionGauge.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CPassionGauge::CPassionGauge(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_eCurState(Gauge::GS_END)
	, m_ePreState(Gauge::GS_END)
	, m_fGaugeValue(0)
	, m_fMaxPassionGaugeValue(0.f)
	, m_fMaxFaithGaugeValue(0.f)
	, m_pPixelShader(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
	ZeroMemory(&m_tColor, sizeof(D3DXCOLOR));
}

CPassionGauge::~CPassionGauge()
{
}

HRESULT CPassionGauge::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_PixelShader();

	m_pTransformCom->Set_Scale(30.f * 2.f, 30.f * 2.f, 0.f);
	m_pTransformCom->Set_Pos(-345.f - 240.f, 245.f + 60.f, 0.12f);

	

	return S_OK;
}

HRESULT CPassionGauge::Ready_Material()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);

	tMtrl.Emissive = m_tColor;
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

HRESULT CPassionGauge::Ready_PixelShader()
{
	LPD3DXBUFFER pCode = NULL; 
	LPD3DXBUFFER pError = NULL; 

	// HLSL 파일 컴파일 
	HRESULT hr = D3DXCompileShaderFromFile( 
		L"../Shader/Gage.hlsl", // 파일명 
		NULL, // 매크로 
		NULL, // include 
		"PS_Gauge", // 엔트리 포인트 
		"ps_2_0", // 셰이더 모델 
		0, // 플래그 
		&pCode, 
		&pError, 
		NULL ); 

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

void CPassionGauge::Update_State()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
			case Gauge::GS_PASSION:
				m_tColor = D3DXCOLOR(0.8f, 0.f, 0.f, 1.f);
				break;
			case Gauge::GS_FAITH:
				m_tColor = D3DXCOLOR(0.1255f, 0.8784f, 0.4392f, 1.0f);
				break;
			default:
				break;
		}
		m_ePreState = m_eCurState;
	}
}

void CPassionGauge::Update_ClampGageValue()
{
	switch (m_eCurState)
	{
		case Gauge::GS_PASSION:
			m_fClampGaugeValue = m_fGaugeValue / m_fMaxPassionGaugeValue;
			break;
		case Gauge::GS_FAITH:
			m_fClampGaugeValue = m_fGaugeValue / m_fMaxFaithGaugeValue;
			break;
		default:
			break;
	}
}

_int CPassionGauge::Update_GameObject(const _float& fTimeDelta)
{
	Update_State();

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CPassionGauge::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);

	Update_ClampGageValue();
}

void CPassionGauge::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pGraphicDev->SetPixelShader(m_pPixelShader);
	m_pGraphicDev->SetPixelShaderConstantF(0, &m_fClampGaugeValue, 1);
	m_pGraphicDev->SetPixelShaderConstantF(1, (float*)m_tColor, 1);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetPixelShader(NULL);
}

void CPassionGauge::OnCollision(CGameObject* pObject)
{

}

HRESULT CPassionGauge::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PassionGauge"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CPassionGauge* CPassionGauge::Create(LPDIRECT3DDEVICE9 pGraphicDev, Gauge::GAUGESTATE eState, _float fPassionMax, _float fFaithMax)
{
	CPassionGauge* pPassionGauge = new CPassionGauge(pGraphicDev);

	pPassionGauge->m_eCurState = eState;
	pPassionGauge->m_fMaxFaithGaugeValue = fFaithMax;
	pPassionGauge->m_fMaxPassionGaugeValue = fPassionMax;


	if (FAILED(pPassionGauge->Ready_GameObject()))
	{
		Safe_Release(pPassionGauge);
		MSG_BOX("pPassionGauge Create Failed");
		return nullptr;
	}

	return pPassionGauge;
}

void CPassionGauge::Free()
{
	CUi::Free();
}