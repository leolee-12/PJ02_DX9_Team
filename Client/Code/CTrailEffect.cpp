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


	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return NOEVENT;
}

void CTrailEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CTrailEffect::Render_GameObject()
{
	if (m_dequePoints.size() < 2) return;

	vector<VTXCOL> vecVtxCol;
	size_t iSize = m_dequePoints.size();

	for(_uint i = 0; i < iSize; ++i)
	{
		_float fRatio = (_float)i / (m_dequePoints.size() - 1);
		_float fAlpha = MyLerp(m_fAlphaStart, m_fAlphaEnd, fRatio);
		_float fWidth = m_fWidth * fRatio;

		_vec3 vDir;

		if (i < iSize - 1)
			vDir = m_dequePoints[i + 1] - m_dequePoints[i];
		else
			vDir = m_dequePoints[i] - m_dequePoints[i - 1];

		D3DXVec3Normalize(&vDir, &vDir);

		_vec3 vUp{ 0.f, 1.f, 0.f };
		_vec3 vRight{};
		D3DXVec3Cross(&vRight, &vDir, &vUp);
		D3DXVec3Normalize(&vRight, &vRight);

		DWORD dwColor = D3DCOLOR_ARGB(_uint(fAlpha * 255), GetRValue(m_dwColor), GetGValue(m_dwColor), GetBValue(m_dwColor));

		VTXCOL vtx1, vtx2;
		vtx1.vPosition = m_dequePoints[i] + vRight * fWidth;
		vtx2.vPosition = m_dequePoints[i] - vRight * fWidth;
		vtx1.dwColor = vtx2.dwColor = dwColor;

		vecVtxCol.push_back(vtx1);
		vecVtxCol.push_back(vtx2);
	}

	m_pGraphicDev->SetFVF(FVF_COL);
	m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, _uint(vecVtxCol.size() - 2), vecVtxCol.data(), sizeof(VTXCOL));
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

CTrailEffect* CTrailEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrailEffect* pTrailEffect = new CTrailEffect(pGraphicDev);

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

	if (FAILED(pTrailEffect->Ready_GameObject()))
	{
		Safe_Release(pTrailEffect);
		MSG_BOX("pTrailEffect Clone Failed");
		return nullptr;
	}

	return pTrailEffect;
}

void CTrailEffect::Free()
{
}
