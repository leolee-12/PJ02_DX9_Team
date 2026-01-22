#include "pch.h"
#include "CTrailEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CTrailEffect::CTrailEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CTrailEffect::CTrailEffect(const CTrailEffect& rhs)
	: CEffect(rhs)
{
}

CTrailEffect::~CTrailEffect()
{
}

HRESULT	CTrailEffect::Ready_GameObject()
{
	return S_OK;
}

_int CTrailEffect::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState != ES_PLAY && m_eState != ES_LOOP) return NOEVENT;

	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return NOEVENT;
}

void CTrailEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Compute_TrailWorldMatrix();
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTrailEffect::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matTrailWorld);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	DWORD dwOldLighting;
	DWORD dwOldSrcBlend, dwOldDestBlend, dwOldTFactor;
	DWORD dwOldColorArg1, dwOldColorArg2, dwOldColorOp;

	m_pGraphicDev->GetRenderState(D3DRS_LIGHTING, &dwOldLighting);
	m_pGraphicDev->GetRenderState(D3DRS_SRCBLEND, &dwOldSrcBlend);
	m_pGraphicDev->GetRenderState(D3DRS_DESTBLEND, &dwOldDestBlend);
	m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTFactor);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwOldColorArg1);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwOldColorArg2);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwOldColorOp);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (m_bEmissive)
	{
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	else
	{
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}

	DWORD dwColor = D3DCOLOR_ARGB(	DWORD(m_tColor.a * 255.f),
									DWORD(m_tColor.r * 255.f),
									DWORD(m_tColor.g * 255.f),
									DWORD(m_tColor.b * 255.f));
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	m_pTextureCom->Set_Texture(0);
	m_pBufferCom->Render_Buffer();

	// 상태 복원
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, dwOldLighting);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, dwOldSrcBlend);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, dwOldDestBlend);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldTFactor);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwOldColorArg1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwOldColorArg2);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwOldColorOp);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CTrailEffect::Play()
{
	m_eState = ES_PLAY;

	OnPlay();
}

void CTrailEffect::Stop()
{
}

void CTrailEffect::Reset()
{
}

void CTrailEffect::Add_Point(const _vec3& vPoint)
{
	_vec3 vGap = vPoint - m_vLastPoint;

	if (m_dequePoints.empty() || D3DXVec3Length(&vGap) >= m_fPointInterval)
	{
		m_dequePoints.push_back(vPoint);
		m_vLastPoint = vPoint;

		if (m_dequePoints.size() > m_iMaxPoints)
		{
			m_dequePoints.pop_front();
		}
	}

}

void CTrailEffect::Compute_TrailWorldMatrix()
{
	// 1. 속도 및 방향 추출
	_float fSpeed = D3DXVec3Length(&m_vSpeed);
	_vec3 vDir;

	if (fSpeed > 0.001f)	D3DXVec3Normalize(&vDir, &m_vSpeed);
	else					vDir = _vec3(0.f, 0.f, -1.f);

	// 2. 꼬리 길이 계산
	_float fLength(0);

	if (m_bScaleBySpeed)	fLength = m_fTailLength * fSpeed;
	else					fLength = m_fTailLength;

	// 3. 빌보드
	_vec3 vRight, vUp(0.f, 1.f, 0.f);
	D3DXVec3Cross(&vRight, &vDir, &vUp);
	D3DXVec3Normalize(&vRight, &vRight);

	// 4. 크기
	_matrix matScale, matRot, matTrans;
	D3DXMatrixScaling(&matScale, fLength, m_fHeadSize, 1.f);

	// 5. 회전
	D3DXMatrixIdentity(&matRot);
	memcpy(matRot.m[0], &vDir, sizeof(_vec3));
	memcpy(matRot.m[1], &vUp, sizeof(_vec3));
	memcpy(matRot.m[2], &vRight, sizeof(_vec3));

	// 6. 이동
	_vec3 vCenter = m_vPos - vDir * (fLength * 0.5f);
	D3DXMatrixTranslation(&matTrans, vCenter.x, vCenter.y, vCenter.z);

	m_matTrailWorld = matScale * matRot * matTrans;
}

CTrailEffect* CTrailEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey)
{
	CTrailEffect* pTrailEffect = new CTrailEffect(pGraphicDev);

	pTrailEffect->m_strProtoTexKey = strProtoTexKey;

	if (FAILED(pTrailEffect->Ready_GameObject()))
	{
		Safe_Release(pTrailEffect);
		MSG_BOX("pTrailEffect Create Failed");
		return nullptr;
	}

	return pTrailEffect;
}

CTrailEffect* CTrailEffect::Clone()
{
	CTrailEffect* pTrailEffect = new CTrailEffect(*this);

	if (FAILED(pTrailEffect->CEffect::Add_Component()))
	{
		Safe_Release(pTrailEffect);
		MSG_BOX("pTrailEffect Clone Failed");
		return nullptr;
	}

	// 런타임 값 초기화
	pTrailEffect->m_eState = ES_READY;
	pTrailEffect->m_fAccTime = 0.f;
	pTrailEffect->m_fEmitAcc = 0.f;
	pTrailEffect->m_vecParticles.clear();
	pTrailEffect->m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);

	return pTrailEffect;
}

void CTrailEffect::Free()
{
}
