#include "pch.h"
#include "CSpriteEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CSpriteEffect::CSpriteEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect(pGraphicDev)
{
}

CSpriteEffect::CSpriteEffect(const CSpriteEffect& rhs)
	: CEffect(rhs)
{
}

CSpriteEffect::~CSpriteEffect()
{
}

HRESULT	CSpriteEffect::Ready_GameObject()
{
	return S_OK;
}

_int CSpriteEffect::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState != ES_PLAY || m_eState != ES_LOOP) return NOEVENT;

	m_fAccTime += fTimeDelta;
	m_fFrame += m_fFrameSpeed * fTimeDelta;
	m_fAlpha -= m_fAlphaDecay * fTimeDelta;
	m_fAlpha = max(0.f, m_fAlpha);

	if (m_fFrame >= m_fFrameEnd)
	{
		if (m_bLoop)
		{
			m_fFrame = 0.f;
			m_eState = ES_LOOP;
			OnLoop();
		}
		else
		{
			m_eState = ES_FINISH;
			OnFinish();
			m_iHp = 0;
		}
	}

	if (m_pOwner) AttachTo(m_pOwner);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return NOEVENT;
}

void CSpriteEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CSpriteEffect::Render_GameObject()
{
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

CSpriteEffect* CSpriteEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _uint iTexIdx)
{
	CSpriteEffect* pEffect = nullptr;// = new CSpriteEffect(pGraphicDev);

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

void CSpriteEffect::Free()
{
}
