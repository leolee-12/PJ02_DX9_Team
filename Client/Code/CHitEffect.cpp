#include "pch.h"
#include "CHitEffect.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"



CHitEffect::CHitEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_fFrame(0.f)
	, m_fFrameEnd(0.f)
	, m_fFrameSpeed(0.f)
	, m_eEffectType(EF_END)
{
}

CHitEffect::CHitEffect(const CHitEffect& rhs)
	: CGameObject(rhs)
	, m_fFrame(0.f)
	, m_fFrameEnd(0.f)
	, m_fFrameSpeed(0.f)
	, m_eEffectType(rhs.m_eEffectType)
{
}

CHitEffect::~CHitEffect()
{
}

HRESULT CHitEffect::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();

	return S_OK;
}

_int CHitEffect::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = m_fFrameEnd - 0.001f;
		return DEAD;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CHitEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	_vec3	vPos;
	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	CGameObject::Compute_ViewDepth(&vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CHitEffect::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CHitEffect::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
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

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_HitEffectTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CHitEffect::Ready_Variable()
{
	m_fFrame = 0.f;
	m_fFrameSpeed = 24.f;

	_float fScale = 10.f;
	m_pTransformCom->Set_Pos(_float(rand() % 20), 0.f, _float(rand() % 20));
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	switch (m_iTexIdx)
	{
	case 0:
		m_fFrameEnd = 8.f;
		break;

	case 1:
		m_fFrameEnd = 8.f;
		break;

	case 2:
		m_fFrameEnd = 6.f;
		break;

	case 3:
		m_fFrameEnd = 5.f;
		break;

	case 4:
		m_fFrameEnd = 12.f;
		break;

	case 5:
		m_fFrameEnd = 18.f;
		break;

	case 6:
		m_fFrameEnd = 9.f;
		break;

	case 7:
		m_fFrameEnd = 5.f;
		break;

	default:
		m_fFrameEnd = 0.f;
		break;
	}
}

void CHitEffect::Set_Texture()
{
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 8;
	_uint iV = iFrame / 8;

	m_matTex._11 = 0.125f;	// 가로는 8칸 고정
	m_matTex._22 = 0.25f;	// 세로는 4칸 고정(Effect)

	m_matTex._31 = _float(iU) * 0.125f;
	m_matTex._32 = _float(iV) * 0.25f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(m_iTexIdx);
}

CHitEffect* CHitEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _uint iTexIdx)
{
	CHitEffect* pEffect = new CHitEffect(pGraphicDev);

	pEffect->m_iTexIdx = iTexIdx;

	if (FAILED(pEffect->Ready_GameObject()))
	{
		Safe_Release(pEffect);
		MSG_BOX("pEffect Create Failed");
		return nullptr;
	}

	pEffect->m_pTransformCom->Set_Pos(vPos.x, vPos.y + Get_Rand_Float(-pEffect->m_fVar, pEffect->m_fVar), vPos.z - 0.01f);
	pEffect->m_pTransformCom->Update_Component(0.f);

	return pEffect;
}

void CHitEffect::Free()
{
	CGameObject::Free();
}
