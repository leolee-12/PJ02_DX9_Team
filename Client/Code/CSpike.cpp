#include "pch.h"
#include "CSpike.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN2_AI.h"

CSpike::CSpike(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CSpike::CSpike(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}


CSpike::CSpike(const CSpike& rhs)
	: CGameObject(rhs),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CSpike::~CSpike()
{
}

HRESULT CSpike::Ready_GameObject()
{
	m_eOBJID = OID_MONSTER;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();

	return S_OK;
}

_int CSpike::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	m_pTransformCom->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);
	// 충돌체 디버그용
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();

		if (m_iRecurCount) Recur_Spike();

		return iExit;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CSpike::LateUpdate_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CSpike::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CSpike::Set_SpikeScale(const _vec3& vScale)
{
	m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);
}

void CSpike::Set_SpikePos(const _vec3& vPos)
{
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

HRESULT CSpike::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SpikeTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CSpike::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = 10.f;
	m_fGroundY = -2.5f + fScale * 0.5f;

	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 20), 1.f, _float(rand() % 20));
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	// Anim 관련 세팅
	m_fFrameEnd = 16.f;
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

void CSpike::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;
		m_iHp = 0;
	}
}

void CSpike::Set_Texture()
{
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 8;
	_uint iV = iFrame / 8;

	m_matTex._11 = 0.125f;	// 가로는 8칸 고정
	m_matTex._22 = 0.25f;	// 세로는 4칸 고정(Spike)
	m_matTex._31 = _float(iU) * 0.125f;
	m_matTex._32 = _float(iV) * 0.25f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(m_iTexIdx);
}

void CSpike::Recur_Spike()
{
	_vec3 vPos{ m_vPos.x, 0.f, m_vPos.z };

	CGameObject* pSpike = CSpike::Create(m_pGraphicDev, m_pMessageChannel, m_vPos, m_vSpeed, --m_iRecurCount, Get_Rand_Int(0, 3));

	if (pSpike)
	{
		wstring strObjTag = L"Spike" + to_wstring(m_iRecurCount);

		IMessageChannel::EVENT ESpike;
		ESpike.strType = L"Obj.Add";
		ESpike.eOBJID = Engine::OID_PROJECTILE;
		ESpike.hmapData.emplace(L"Obj", pSpike);
		ESpike.hmapData.emplace(L"LayerTag", L"GameLogic_Layer");
		ESpike.hmapData.emplace(L"ObjTag", strObjTag);
		m_pMessageChannel->Publish(ESpike);
	}
}

CSpike* CSpike::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos, _vec3 vSpeed, _uint iRecurred, _uint iTexIdx)
{
	CSpike* pSpike = new CSpike(pGraphicDev, StageChannel);

	if (FAILED(pSpike->Ready_GameObject()))
	{
		Safe_Release(pSpike);
		MSG_BOX("pSpike Create Failed");
		return nullptr;
	}

	pSpike->m_pTransformCom->Set_Pos(vPos.x, pSpike->m_fGroundY, vPos.z);
	pSpike->m_vSpeed = vSpeed;
	pSpike->m_iTexIdx = iTexIdx;
	pSpike->m_iRecurCount = iRecurred;

	return pSpike;
}

void CSpike::Free()
{
	CGameObject::Free();
}
