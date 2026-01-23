#include "pch.h"
#include "CIndicator.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CIndicator::CIndicator(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CIndicator::CIndicator(const CIndicator& rhs)
	: CEffect(rhs)
{
}

CIndicator::~CIndicator()
{
}

HRESULT	CIndicator::Ready_GameObject()
{
	FAILED_CHECK_RETURN(CEffect::Add_Component(), E_FAIL);

	m_eType = EF_SPRITE;
	m_eState = ES_READY;

	return S_OK;
}

_int CIndicator::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState != ES_PLAY && m_eState != ES_LOOP) return NOEVENT;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_TILE, this);

	return iExit;
}

void CIndicator::LateUpdate_GameObject(const _float& fTimeDelta)
{
	_matrix matScale, matRot, matTrans;

	D3DXMatrixScaling(&matScale, m_vScale.x, m_vScale.y, m_vScale.z);

	// XZ 평면에 눕히기 (X축 90도 회전)
	D3DXMatrixRotationX(&matRot, D3DX_PI * 0.5f);

	D3DXMatrixTranslation(&matTrans,
		m_vLandingPos.x,
		m_vLandingPos.y + 0.01f,  // Z-fighting 방지
		m_vLandingPos.z);

	m_matWorld = matScale * matRot * matTrans;

	// 뷰 뎁스는 바닥 기준
	Compute_ViewDepth(&m_vLandingPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CIndicator::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x08);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CIndicator::Play()
{
	m_eState = ES_PLAY;
	m_fAccTime = 0.f;
	OnPlay();
}

void CIndicator::Stop()
{
}

void CIndicator::Reset()
{
}

void CIndicator::Update_OwnerData(const _vec3& vPos, const _vec3& vSpeed, const _float& fGravity, const _float& fGroundY)
{
	m_vLandingPos = Compute_LandingPos(vPos, vSpeed, fGravity, fGroundY);
}

_vec3 CIndicator::Compute_LandingPos(const _vec3& vPos, const _vec3& vSpeed, const _float& fGravity, const _float& fGroundY)
{
	// 이미 바닥 아래면 현재 위치 반환
	if (vPos.y <= fGroundY)
		return _vec3(vPos.x, fGroundY, vPos.z);

	// 이차방정식: 0.5*g*t² + vy*t + (y0 - groundY) = 0
	_float a = 0.5f * fGravity;
	_float b = vSpeed.y;
	_float c = vPos.y - fGroundY;

	_float discriminant = b * b - 4.f * a * c;

	// 판별식이 음수면 바닥에 도달하지 않음
	if (discriminant < 0.f)
		return _vec3(vPos.x, fGroundY, vPos.z);  // 기본값

	// 양의 근 선택 (미래 시간)
	_float t = (-b + sqrtf(discriminant)) / (2.f * a);

	if (t < 0.f)
		t = (-b - sqrtf(discriminant)) / (2.f * a);

	if (t < 0.f)
		return _vec3(vPos.x, fGroundY, vPos.z);  // 기본값

	// 착탄 위치 계산
	_vec3 vLandingPos;
	vLandingPos.x = vPos.x + vSpeed.x * t;
	vLandingPos.y = fGroundY;
	vLandingPos.z = vPos.z + vSpeed.z * t;

	return vLandingPos;
}

CIndicator* CIndicator::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey)
{
	CIndicator* pIndicator = new CIndicator(pGraphicDev);

	pIndicator->m_strProtoTexKey = strProtoTexKey;

	if (FAILED(pIndicator->Ready_GameObject()))
	{
		Safe_Release(pIndicator);
		MSG_BOX("pIndicator Create Failed");
		return nullptr;
	}

	return pIndicator;
}

CIndicator* CIndicator::Clone()
{
	CIndicator* pIndicator = new CIndicator(*this);

	if (FAILED(pIndicator->Add_Component()))
	{
		Safe_Release(pIndicator);
		MSG_BOX("pIndicator Clone Failed");
		return nullptr;
	}

	pIndicator->m_eState = ES_READY;
	pIndicator->m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);
	pIndicator->m_vScale = m_vScale;

	return pIndicator;
}

void CIndicator::Free()
{
	CEffect::Free();
}
