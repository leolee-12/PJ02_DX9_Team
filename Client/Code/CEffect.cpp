#include "pch.h"
#include "CEffect.h"
#include "CProtoMgr.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CGameObject(pGraphicDev)
		, m_pBufferCom(nullptr)
		, m_pTransformCom(nullptr)
		, m_pTextureCom(nullptr)
		, m_eType(EF_END)
		, m_eState(ES_READY)
		, m_fLifeTime(0.f)
		, m_fAccTime(0.f)
		, m_bLoop(false)
		, m_pOwner(nullptr)
		, m_iTexIdx(0)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_eType(rhs.m_eType)
	, m_eState(ES_READY)
	, m_fLifeTime(rhs.m_fLifeTime)
	, m_fAccTime(0.f)
	, m_bLoop(rhs.m_bLoop)
	, m_pOwner(nullptr)
	, m_iTexIdx(rhs.m_iTexIdx)
{
	m_strProtoTexKey = rhs.m_strProtoTexKey;
}

CEffect::~CEffect()
{
}

void CEffect::AttachTo(CGameObject* pOwner, const _vec3& vOffset)
{
	if (!pOwner) return;

	CTransform*	pTransformCom = static_cast<CTransform*>(pOwner->Get_Component(ID_DYNAMIC, L"Com_Transform"));

	if (!pTransformCom) CTransform* pTransformCom = static_cast<CTransform*>(pOwner->Get_Component(ID_STATIC, L"Com_Transform"));

	if (!pTransformCom) return;

	_vec3 vPos;
	pTransformCom->Get_Info(INFO_POS, &vPos);
	vPos += vOffset;
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CEffect::Detach()
{
	m_pOwner = nullptr;
}

HRESULT CEffect::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(m_strProtoTexKey));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CEffect::Free()
{
	CGameObject::Free();
}
