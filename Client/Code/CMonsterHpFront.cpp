#include "pch.h"
#include "CMonsterHpFront.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CFontMgr.h"

CMonsterHpFront::CMonsterHpFront(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CMonsterHpFront::~CMonsterHpFront()
{
}

HRESULT CMonsterHpFront::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_PixelShader();

	m_pTransformCom->Set_Scale(59.f, 7.f, 0.1f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	m_fRatio = 1.0f;

	return S_OK;
}

_int CMonsterHpFront::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CMonsterHpFront::LateUpdate_GameObject(const _float& fTimeDelta)
{

	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CMonsterHpFront::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetPixelShader(m_pPixelShader);
	m_pGraphicDev->SetPixelShaderConstantF(0, &m_fLerpHp, 1);
	m_pGraphicDev->SetPixelShaderConstantF(1, &m_fRatio, 1);


	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetPixelShader(NULL);

	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	wchar_t szGauge[128];

	swprintf_s(szGauge, L" Front : %.3f", m_fRatio);
	RECT rcPlayer = { 0, 0, 200, 300 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szGauge, rcPlayer, FontColor, DT_RIGHT | DT_BOTTOM);

	D3DXCOLOR FontColor1 = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	wchar_t szGauge1[128];


	rcPlayer = { 0, 0, 200, 200 };
	swprintf_s(szGauge1, L" Front : %.3f", m_fLerpHp);
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szGauge1, rcPlayer, FontColor1, DT_RIGHT | DT_BOTTOM);
}

void CMonsterHpFront::OnCollision(CGameObject* pObject)
{

}

HRESULT CMonsterHpFront::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MonsterHpFront"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CMonsterHpFront::Set_Pos(const _vec3& vPos)
{
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

HRESULT CMonsterHpFront::Ready_PixelShader()
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


CMonsterHpFront* CMonsterHpFront::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos)
{
	CMonsterHpFront* pBossHpBarFront = new CMonsterHpFront(pGraphicDev);

	pBossHpBarFront->m_vPos = _vPos;
	//pBossHpBarFront->m_fScale = _fScale;


	if (FAILED(pBossHpBarFront->Ready_GameObject()))
	{
		Safe_Release(pBossHpBarFront);
		MSG_BOX("pMonsterHpBarFront Create Failed");
		return nullptr;
	}
	return pBossHpBarFront;
}

void CMonsterHpFront::Free()
{
	m_pPixelShader->Release();
	CUi::Free();
}
