#include "pch.h"
#include "CParticleEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CParticleEffect::CParticleEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CParticleEffect::CParticleEffect(const CParticleEffect& rhs)
	: CEffect(rhs)
{
}

CParticleEffect::~CParticleEffect()
{
}

HRESULT	CParticleEffect::Ready_GameObject()
{
	return S_OK;
}

_int CParticleEffect::Update_GameObject(const _float& fTimeDelta)
{
	// 1. 파티클 생성
	if (m_eState == ES_PLAY)
	{
		m_fEmitAcc += m_fEmitRate * fTimeDelta;
		size_t iMaxCount = m_vecParticles.size();

		while (m_fEmitAcc >= 1.f && iMaxCount < m_iMaxParticles)
		{
			Play();
			m_fEmitAcc -= 1.f;
		}
	}

	// 2. 파티클 업데이트
	for (auto iter = m_vecParticles.begin(); iter != m_vecParticles.end();)
	{
		iter->fLife -= fTimeDelta;

		if (iter->fLife <= 0.f)
		{
			iter = m_vecParticles.erase(iter);
			continue;
		}

		// 물리
		iter->vSpeed += m_vGravity * fTimeDelta;		// 중력
		iter->vSpeed *= (1.f - m_fDrag * fTimeDelta);	// 공기저항
		iter->vPos += iter->vSpeed * fTimeDelta;		// 움직임

		// 보간
		_float fRatio = 1.f - (iter->fLife / iter->fMaxLife);
		iter->fSize = MyLerp(m_fSizeStart, m_fSizeEnd, fRatio);
		iter->fAlpha = MyLerp(m_fAlphaStart, m_fAlphaEnd, fRatio);

		iter++;
	}

	// 3. 완료 체크
	if (m_eState == ES_PLAY && m_fAccTime >= m_fLifeTime)
	{
		m_eState = ES_FINISH;

	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return NOEVENT;
}

void CParticleEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CParticleEffect::Render_GameObject()
{
}

void CParticleEffect::Play()
{
	m_eState = ES_PLAY;
	OnPlay();
}

void CParticleEffect::Stop()
{
}

void CParticleEffect::Reset()
{
}

CParticleEffect* CParticleEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _uint iTexIdx)
{
	CParticleEffect* pEffect = nullptr;// = new CParticleEffect(pGraphicDev);

	//pEffect->m_iTexIdx = iTexIdx;

	//if (FAILED(pEffect->Ready_GameObject()))
	//{
	//	Safe_Release(pEffect);
	//	MSG_BOX("pEffect Create Failed");
	//	return nullptr;
	//}

	//pEffect->m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z - 0.01f);
	//pEffect->m_pTransformCom->Update_Component(0.f);

	return pEffect;
}

void CParticleEffect::Free()
{
}
