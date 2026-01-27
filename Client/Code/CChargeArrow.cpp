#include "pch.h"
#include "CChargeArrow.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CChargeArrow::CChargeArrow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CChargeArrow::CChargeArrow(const CChargeArrow& rhs)
	: CEffect(rhs)
	, m_pPixelShader(rhs.m_pPixelShader)
	, m_fRatio(rhs.m_fRatio)
{
}

CChargeArrow::~CChargeArrow()
{
}

HRESULT	CChargeArrow::Ready_GameObject()
{
	FAILED_CHECK_RETURN(CEffect::Add_Component(), E_FAIL);

	m_eType = EF_INDICATOR;     // 바닥 이펙트 타입
	m_eState = ES_READY;
	m_vDir = _vec3(0.f, 0.f, 1.f);
	_float fScale = 6.f;
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	m_vDepth = { 0.f, 0.f, FLT_MAX };

	if (FAILED(Ready_PixelShader()))
		return E_FAIL;

	return S_OK;
}

_int CChargeArrow::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState != ES_PLAY) return NOEVENT;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	//CRenderer::GetInstance()->Add_RenderGroup(RENDER_TILE, this);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	_tchar szPos[256] = L"";
	swprintf_s(szPos, L"[차지에로우] Ratio값 : %.2f", m_fRatio);
	OutputDebugString(szPos);
	OutputDebugString(L"\n");

	return NOEVENT;
}

void CChargeArrow::LateUpdate_GameObject(const _float& fTimeDelta)
{
	_matrix matScale, matRotX, matRotY, matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);

	// XZ 평면에 눕히기 (X축 90도 회전)
	D3DXMatrixRotationX(&matRotX, D3DX_PI * 0.5f);

	// 방향 회전
	_float fAngleY = atan2f(m_vDir.x, m_vDir.z) - D3DX_PI * 0.5f;
	D3DXMatrixRotationY(&matRotY, fAngleY);

	// 이동
	_vec3 vPos;
	_float fHalfLength = m_vScale.x * 0.5f;
	_float fXPosOffset = m_vDir.x * fHalfLength;
	_float fZPosOffset = m_vDir.z * fHalfLength;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	D3DXMatrixTranslation(&matTrans, vPos.x + fXPosOffset, -2.39f, vPos.z + fZPosOffset);

	m_matWorld = matScale * matRotX * matRotY * matTrans;

	// 뷰 뎁스는 바닥 기준
	//Compute_ViewDepth(&vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CChargeArrow::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);

	m_pGraphicDev->SetPixelShader(m_pPixelShader);
	m_pGraphicDev->SetPixelShaderConstantF(0, &m_fRatio, 1);

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetPixelShader(NULL);
}

void CChargeArrow::Play()
{
	m_eState = ES_PLAY;
}

void CChargeArrow::Stop()
{
	m_eState = ES_READY;
}

void CChargeArrow::Reset()
{
	m_eState = ES_READY;
	m_vDir = _vec3(0.f, 0.f, 1.f);
}

void CChargeArrow::Update_OwnerData(const _vec3& vPos, const _vec3& vDir, _float fRatio)
{
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);  // 발 아래
	m_vDir = vDir;
	m_fRatio = fRatio;
}

HRESULT CChargeArrow::Ready_PixelShader()
{
	LPD3DXBUFFER pCode = NULL;
	LPD3DXBUFFER pError = NULL;

	// HLSL 파일 컴파일 
	HRESULT hr = D3DXCompileShaderFromFile(
		L"../Shader/Arrow.hlsl", // 파일명 
		NULL, // 매크로 
		NULL, // include 
		"PS_Arrow", // 엔트리 포인트 
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

CChargeArrow* CChargeArrow::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey)
{
	CChargeArrow* pChargeArrow = new CChargeArrow(pGraphicDev);

	pChargeArrow->m_strProtoTexKey = strProtoTexKey;

	if (FAILED(pChargeArrow->Ready_GameObject()))
	{
		Safe_Release(pChargeArrow);
		MSG_BOX("pChargeArrow Create Failed");
		return nullptr;
	}

	return pChargeArrow;
}

CChargeArrow* CChargeArrow::Clone()
{
	CChargeArrow* pChargeArrow = new CChargeArrow(*this);

	if (FAILED(pChargeArrow->Add_Component()))
	{
		Safe_Release(pChargeArrow);
		MSG_BOX("pChargeArrow Clone Failed");
		return nullptr;
	}

	pChargeArrow->m_eState = ES_READY;
	pChargeArrow->m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);
	pChargeArrow->m_vScale = m_vScale;

	return pChargeArrow;
}

void CChargeArrow::Free()
{
	if (!m_bClone) { Safe_Release(m_pPixelShader); }
	CEffect::Free();
}
