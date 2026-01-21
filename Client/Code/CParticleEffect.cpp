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
	, m_iMaxParticles(rhs.m_iMaxParticles)
	, m_fEmitRate(rhs.m_fEmitRate)
	, m_vEmitMinPos(rhs.m_vEmitMinPos)
	, m_vEmitMaxPos(rhs.m_vEmitMaxPos)
	, m_vMinSpeed(rhs.m_vMinSpeed)
	, m_vMaxSpeed(rhs.m_vMaxSpeed)
	, m_vGravity(rhs.m_vGravity)
	, m_fDrag(rhs.m_fDrag)
	, m_fSizeStart(rhs.m_fSizeStart)
	, m_fSizeEnd(rhs.m_fSizeEnd)
	, m_fAlphaStart(rhs.m_fAlphaStart)
	, m_fAlphaEnd(rhs.m_fAlphaEnd)
	, m_iMinTexIdx(rhs.m_iMinTexIdx)
	, m_iMaxTexIdx(rhs.m_iMaxTexIdx)
	, m_bFull(false)
	, m_tBaseColor(rhs.m_tBaseColor)
{
	m_vecParticles.reserve(m_iMaxParticles);
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
	m_vGravity = _vec3(0.f, -9.8f * 2.5f, 0.f);
	m_fDrag = 0.98f;

	m_fSizeStart = 1.5f;
	m_fSizeEnd = 0.1f;
	m_fAlphaStart = 1.f;
	m_fAlphaEnd = 0.f;

	m_vecParticles.reserve(m_iMaxParticles);

	return S_OK;
}

_int CParticleEffect::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState != ES_PLAY && m_eState != ES_LOOP) return NOEVENT;

	// 1. 파티클 생성
	m_fAccTime += fTimeDelta;
	m_fEmitAcc += m_fEmitRate * fTimeDelta;

	while (!m_bFull && m_fEmitAcc >= 0.05f && m_vecParticles.size() < m_iMaxParticles)
	{
		Emit_Particle();
		m_fEmitAcc -= 0.05f;

		if (m_vecParticles.size() >= m_iMaxParticles)
			m_bFull = true;
	}

	// 2. 파티클 업데이트
	for (auto iter = m_vecParticles.begin(); iter != m_vecParticles.end();)
	{
		Particle& p = *iter;

		p.fLife -= fTimeDelta;

		if (p.fLife <= 0.f)
		{
			iter = m_vecParticles.erase(iter);
			continue;
		}

		// 물리
		p.vSpeed += m_vGravity * fTimeDelta;		// 중력
		//p.vSpeed *= (1.f - m_fDrag * fTimeDelta);	// 공기저항
		p.vPos += iter->vSpeed * fTimeDelta;		// 움직임

		// 바닥 충돌 (y = -2.5f)
		if (p.vPos.y < -2.5f)
		{
			p.vPos.y = 0.f;
			p.vSpeed.y *= -0.3f;  // 바운스
			p.vSpeed.x *= 0.4f;   // 마찰
			p.vSpeed.z *= 0.4f;
		}

		// 보간
		_float fRatio = 1.f - (iter->fLife / iter->fMaxLife);
		iter->fSize = MyLerp(m_fSizeStart, m_fSizeEnd, fRatio);
		iter->fAlpha = MyLerp(m_fAlphaStart, m_fAlphaEnd, fRatio);

		iter++;
	}

	// 3. 완료 체크
	if (m_bFull && m_vecParticles.empty())
	{
		m_eState = ES_FINISH;
		OnFinish();
		return DEAD;
	}

	CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return NOEVENT;
}

void CParticleEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CParticleEffect::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	DWORD dwOldFactor, dwOldAArg1, dwOldAArg2, dwOldAOp, dwOldCArg1, dwOldCArg2, dwOldCOp;
	m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldFactor);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwOldAArg1);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAARG2, &dwOldAArg2);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwOldAOp);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwOldCArg1);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwOldCArg2);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwOldCOp);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	for (auto& p : m_vecParticles)
	{
		// 1. UV 설정
		Set_Texture(p.iTexIdx);

		// 2. 빌보드 월드 행렬
		_matrix matWorld;
		Compute_ParticleBBD(&matWorld, p.vPos, p.fSize);
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		// 3. 알파 설정 (TextureFactor 방식)

		//DWORD dwAlpha = DWORD(p.fAlpha * 255.f);
		DWORD dwColor = D3DCOLOR_ARGB(	DWORD(p.fAlpha * 255.f),
										DWORD(p.tColor.r * 255.f),
										DWORD(p.tColor.g * 255.f),
										DWORD(p.tColor.b * 255.f));

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);
		//m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(dwAlpha, 255, 255, 255));

		// 4. 쿼드 렌더링
		m_pBufferCom->Render_Buffer();
	}

	// 상태 복원
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldFactor);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwOldAArg1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, dwOldAArg2);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwOldAOp);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwOldCArg1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwOldCArg2);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwOldCOp);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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
	p.fMaxLife = p.fLife = m_fLifeTime + Get_Rand_Float(-0.5f, 0.5f);
	p.fSize = m_fSizeStart;
	p.fAlpha = m_fAlphaStart;
	p.iTexIdx = Get_Rand_Int(m_iMinTexIdx, m_iMaxTexIdx);
	p.tColor = m_tBaseColor;
	//p.tColor.r = m_tBaseColor.r + Get_Rand_Float(-m_fColorVariance, m_fColorVariance);
	//p.tColor.g = m_tBaseColor.g + Get_Rand_Float(-m_fColorVariance, m_fColorVariance);
	//p.tColor.b = m_tBaseColor.b + Get_Rand_Float(-m_fColorVariance, m_fColorVariance);
	//p.tColor.r = max(0.f, min(1.f, p.tColor.r));
	//p.tColor.g = max(0.f, min(1.f, p.tColor.g));
	//p.tColor.b = max(0.f, min(1.f, p.tColor.b));
	//p.tColor.a = 1.f;

	m_vecParticles.push_back(p);
}

void CParticleEffect::Set_Texture(const _uint& iTexIdx)
{
	_uint iU = iTexIdx % GRID_X;
	_uint iV = iTexIdx / GRID_X;

	D3DXMatrixIdentity(&m_matTex);
	m_matTex._11 = 1.f / GRID_X;          // U 스케일 (0.125)
	m_matTex._22 = 1.f / GRID_Y;          // V 스케일 (0.25)
	m_matTex._31 = iU / (_float)GRID_X;   // U 오프셋
	m_matTex._32 = iV / (_float)GRID_Y;   // V 오프셋

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);
	m_pTextureCom->Set_Texture(0);
}

void CParticleEffect::Compute_ParticleBBD(_matrix* pOut, const _vec3& vPos, const _float& fSize)
{
	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, nullptr, &matView);
	D3DXMatrixIdentity(pOut);

	// 뷰 행렬의 회전 부분만 추출 (X축 빌보드)
	pOut->_11 = matView._11;
	pOut->_12 = matView._12;
	pOut->_13 = matView._13;
	pOut->_21 = 0.f;         // Y축 고정
	pOut->_22 = 1.f;
	pOut->_23 = 0.f;
	pOut->_31 = matView._31;
	pOut->_32 = matView._32;
	pOut->_33 = matView._33;

	// 스케일 적용
	pOut->_11 *= fSize;
	pOut->_22 *= fSize;
	pOut->_33 *= fSize;

	// 위치
	pOut->_41 = vPos.x;
	pOut->_42 = vPos.y;
	pOut->_43 = vPos.z;
}

CParticleEffect* CParticleEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey)
{
	CParticleEffect* pParticleEffect = new CParticleEffect(pGraphicDev);

	pParticleEffect->m_strProtoTexKey = strProtoTexKey;

	if (FAILED(pParticleEffect->Ready_GameObject()))
	{
		Safe_Release(pParticleEffect);
		MSG_BOX("pParticleEffect Create Failed");
		return nullptr;
	}

	return pParticleEffect;
}

CParticleEffect* CParticleEffect::Clone()
{
	CParticleEffect* pParticleEffect = new CParticleEffect(*this);

	if (FAILED(pParticleEffect->CEffect::Add_Component()))
	{
		Safe_Release(pParticleEffect);
		MSG_BOX("pParticleEffect Clone Failed");
		return nullptr;
	}

	// 런타임 값 초기화
	pParticleEffect->m_eState = ES_READY;
	pParticleEffect->m_fAccTime = 0.f;
	pParticleEffect->m_fEmitAcc = 0.f;
	pParticleEffect->m_vecParticles.clear();
	pParticleEffect->m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);

	return pParticleEffect;
}

void CParticleEffect::Free()
{
	CEffect::Free();
}
