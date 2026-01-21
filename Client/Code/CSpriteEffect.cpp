#include "pch.h"
#include "CSpriteEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CSpriteEffect::CSpriteEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
	, m_bBillboard(false)
	, m_fAlphaDecay(0.f)
{
	ZeroMemory(&m_tSpriteData, sizeof(m_tSpriteData));
}

CSpriteEffect::CSpriteEffect(const CSpriteEffect& rhs)
	: CEffect(rhs)
	, m_bBillboard(rhs.m_bBillboard)
	, m_fAlphaDecay(rhs.m_fAlphaDecay)
{
	memcpy(&m_tSpriteData, &rhs.m_tSpriteData, sizeof(SPRITE_DATA));
}

CSpriteEffect::~CSpriteEffect()
{
}

HRESULT	CSpriteEffect::Ready_GameObject()
{
	FAILED_CHECK_RETURN(CEffect::Add_Component(), E_FAIL);

	m_eType = EF_SPRITE;
	m_eState = ES_READY;
	m_fCurFrame = 0.f;
	m_bBillboard = true;

	return S_OK;
}

_int CSpriteEffect::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState != ES_PLAY && m_eState != ES_LOOP) return NOEVENT;

	m_fAccTime += fTimeDelta;
	m_fFrame += m_tSpriteData.fFrameSpeed * fTimeDelta;
	m_fAlpha -= m_fAlphaDecay * fTimeDelta;
	m_fAlpha = max(0.f, m_fAlpha);

	// Anim 체크
	if (m_fFrame >= _float(m_tSpriteData.iFrameEnd))
	{
		if (m_bLoop)
		{
			m_fFrame = 0.f;
			m_eState = ES_LOOP;
			OnLoop();
		}
		else
		{
			m_fFrame = _float(m_tSpriteData.iFrameEnd) - 0.001f;
			m_eState = ES_FINISH;
			OnFinish();
			return DEAD;
		}
	}

	// 부착
	if (m_pOwner) AttachTo(m_pOwner);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CSpriteEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bBillboard)
		m_pTransformCom->Compute_Bilboard(BBD_X);

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	vPos.z -= 0.2f + 0.001f * m_iTexIdx;
	Compute_ViewDepth(&vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CSpriteEffect::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 스프라이트 시트 UV 설정
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CSpriteEffect::Play()
{
	m_eState = ES_PLAY;
	m_fFrame = 0.f;
	m_fAccTime = 0.f;
	m_fAlpha = 1.f;
	OnPlay();
}

void CSpriteEffect::Stop()
{
}

void CSpriteEffect::Reset()
{
}

void CSpriteEffect::Set_Texture()
{
	_uint iFrame = _uint(m_fFrame);
	_uint iU = iFrame % m_tSpriteData.iGridX;
	_uint iV = iFrame / m_tSpriteData.iGridX;

	D3DXMatrixIdentity(&m_matTex);
	m_matTex._11 = 1.f / _float(m_tSpriteData.iGridX);
	m_matTex._22 = 1.f / _float(m_tSpriteData.iGridY);
	m_matTex._31 = _float(iU) / _float(m_tSpriteData.iGridX);
	m_matTex._32 = _float(iV) / _float(m_tSpriteData.iGridY);

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(m_iTexIdx);
}

CSpriteEffect* CSpriteEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& strProtoTexKey)
{
	CSpriteEffect* pSpriteEffect = new CSpriteEffect(pGraphicDev);

	pSpriteEffect->m_strProtoTexKey = strProtoTexKey;

	if (FAILED(pSpriteEffect->Ready_GameObject()))
	{
		Safe_Release(pSpriteEffect);
		MSG_BOX("pSpriteEffect Create Failed");
		return nullptr;
	}

	return pSpriteEffect;
}

CSpriteEffect* CSpriteEffect::Clone()
{
	CSpriteEffect* pSpriteEffect = new CSpriteEffect(*this);

	if (FAILED(pSpriteEffect->Add_Component()))
	{
		Safe_Release(pSpriteEffect);
		MSG_BOX("pSpriteEffect Clone Failed");
		return nullptr;
	}

	pSpriteEffect->m_eState = ES_READY;
	pSpriteEffect->m_fFrame = 0.f;
	pSpriteEffect->m_fAlpha = 1.f;
	pSpriteEffect->m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, m_vScale.z);

	return pSpriteEffect;
}

void CSpriteEffect::Free()
{
	CEffect::Free();
}
