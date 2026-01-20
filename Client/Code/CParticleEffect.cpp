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
	FAILED_CHECK_RETURN(CEffect::Add_Component(), E_FAIL);

	m_eType = EF_PARTICLE;
	m_eState = ES_READY;

	// 기본 설정
	m_iMaxParticles = 50;
	m_fEmitRate = 10.f;
	m_fEmitAcc = 0.f;
	m_vGravity = _vec3(0.f, -9.8f, 0.f);
	m_fDrag = 0.98f;

	m_fSizeStart = 0.5f;
	m_fSizeEnd = 0.1f;
	m_fAlphaStart = 1.f;
	m_fAlphaEnd = 0.f;

	m_vecParticles.reserve(m_iMaxParticles);

	return S_OK;
}

_int CParticleEffect::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState != ES_PLAY && m_eState != ES_LOOP) return NOEVENT;
	else	// 1. 파티클 생성
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
		return DEAD;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return NOEVENT;
}

void CParticleEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CParticleEffect::Render_GameObject()
{
	for (auto& p : m_vecParticles)
	{
		// 각 파티클마다 월드 행렬 설정 (빌보드)
		_matrix matWorld;
		D3DXMatrixScaling(&matWorld, p.fSize, p.fSize, p.fSize);

		// 빌보드 회전 적용
		_matrix matView;
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
		matWorld._11 = matView._11; matWorld._13 = matView._13;
		matWorld._31 = matView._31; matWorld._33 = matView._33;

		matWorld._41 = p.vPos.x;
		matWorld._42 = p.vPos.y;
		matWorld._43 = p.vPos.z;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		// 알파 적용
		DWORD dwAlpha = DWORD(p.fAlpha * 255.f);
		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwAlpha, 255, 255, 255));

		m_pTextureCom->Set_Texture(0);
		m_pBufferCom->Render_Buffer();
	}
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

void CParticleEffect::Emit_Particle()
{
	Particle p;
	_vec3 vBasePos;
	m_pTransformCom->Get_Info(INFO_POS, &vBasePos);

	p.vPos = vBasePos + _vec3(
		Get_Rand_Float(m_vEmitMinPos.x, m_vEmitMaxPos.x),
		Get_Rand_Float(m_vEmitMinPos.y, m_vEmitMaxPos.y),
		Get_Rand_Float(m_vEmitMinPos.z, m_vEmitMaxPos.z)
	);
	p.vSpeed = _vec3(
		Get_Rand_Float(m_vMinSpeed.x, m_vMaxSpeed.x),
		Get_Rand_Float(m_vMinSpeed.y, m_vMaxSpeed.y),
		Get_Rand_Float(m_vMinSpeed.z, m_vMaxSpeed.z)
	);
	p.fMaxLife = p.fLife = Get_Rand_Float(0.5f, 1.5f);
	p.fSize = m_fSizeStart;
	p.fAlpha = m_fAlphaStart;
	p.dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	m_vecParticles.push_back(p);
}

CParticleEffect* CParticleEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CParticleEffect* pEffect = nullptr;

	return pEffect;
}

CParticleEffect* CParticleEffect::Clone()
{
	return new CParticleEffect(*this);
}

void CParticleEffect::Free()
{
}
