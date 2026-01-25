#include "pch.h"
#include "CBossHpBarFront.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CFontMgr.h"

CBossHpBarFront::CBossHpBarFront(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CBossHpBarFront::~CBossHpBarFront()
{
}

HRESULT CBossHpBarFront::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_PixelShader();

	m_pTransformCom->Set_Scale(59.f * 8.f, 7.f * 3.f, 0.1f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->Update_Component(0.f);

	m_fRatio = 1.0f;

	return S_OK;
}

_int CBossHpBarFront::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CBossHpBarFront::LateUpdate_GameObject(const _float& fTimeDelta)
{

	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CBossHpBarFront::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetPixelShader(m_pPixelShader);
	m_pGraphicDev->SetPixelShaderConstantF(0, &m_fLerpHp, 1);
	m_pGraphicDev->SetPixelShaderConstantF(1, &m_fRatio, 1);


	m_pTextureCom->Set_Texture();
	
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetPixelShader(NULL);
}

void CBossHpBarFront::OnCollision(CGameObject* pObject)
{

}

HRESULT CBossHpBarFront::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BossHpBar"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

HRESULT CBossHpBarFront::Ready_PixelShader()
{
	LPD3DXBUFFER pCode = NULL;
	LPD3DXBUFFER pError = NULL;

	// HLSL 파일 컴파일 
	HRESULT hr = D3DXCompileShaderFromFile(
		L"../Shader/BossHp.hlsl", // 파일명 
		NULL, // 매크로 
		NULL, // include 
		"PS_BossHp", // 엔트리 포인트 
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


CBossHpBarFront* CBossHpBarFront::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos)
{
	CBossHpBarFront* pBossHpBarFront = new CBossHpBarFront(pGraphicDev);

	pBossHpBarFront->m_vPos = _vPos;
	//pBossHpBarFront->m_fScale = _fScale;


	if (FAILED(pBossHpBarFront->Ready_GameObject()))
	{
		Safe_Release(pBossHpBarFront);
		MSG_BOX("pBossHpBarFront Create Failed");
		return nullptr;
	}
	return pBossHpBarFront;
}

void CBossHpBarFront::Free()
{
	m_pPixelShader->Release();
	CUi::Free();
}
