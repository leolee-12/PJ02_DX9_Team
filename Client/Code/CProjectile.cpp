#include "pch.h"
#include "CProjectile.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"

CProjectile::CProjectile(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CGameObject(pGraphicDev),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_iAttack(0),
		m_fGroundY(0.f),
		m_bActive(true),
		m_fAcmlTime(0.f),
		m_fLifeTime(0.f),
		m_fGravity(0.f),
		m_bUseGravity(false)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CProjectile::CProjectile(const CProjectile& rhs)
	:	CGameObject(rhs),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_iAttack(rhs.m_iAttack),
		m_fGroundY(rhs.m_fGroundY),
		m_bActive(rhs.m_bActive),
		m_fAcmlTime(rhs.m_fAcmlTime),
		m_fLifeTime(rhs.m_fLifeTime),
		m_fGravity(rhs.m_fGravity),
		m_bUseGravity(rhs.m_bUseGravity)
{
}

CProjectile::~CProjectile()
{
}

void CProjectile::Set_Pos(const _vec3& vPos)
{
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

HRESULT CProjectile::Ready_GameObject()
{
	m_eOBJID = OID_MONSTER;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	return S_OK;
}

_int CProjectile::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	m_fAcmlTime += fTimeDelta;

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);
	// 충돌체 디버그용
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (m_fAcmlTime >= m_fLifeTime)
		iExit = DEAD;

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		return iExit;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CProjectile::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bUseGravity) m_vSpeed.y += m_fGravity * fTimeDelta;

	m_pTransformCom->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);
	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CProjectile::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();
}

void CProjectile::OnCollision(CGameObject* pObject)
{
	if (!m_bActive) return;

	if (pObject->Get_OBJID() == OID_PLAYER)
	{
		m_iHp = 0;
		m_bActive = false;
	}
}

HRESULT CProjectile::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ProjectileTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CProjectile::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = 1.f;
	m_fGroundY = -2.5f + fScale * 0.5f;
	m_iAttack = 1;
	m_iHp = 1;
	m_fAcmlTime = 0.f;
	m_fLifeTime = 10.f;
	m_fGravity = -9.8f;

	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 20), m_fGroundY, _float(rand() % 20));
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_MBULLET);

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

void CProjectile::Ready_Event()
{

}

void CProjectile::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;
	}
}

CProjectile* CProjectile::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vSpeed, _bool bUseGravity)
{
	CProjectile* pProjectile = new CProjectile(pGraphicDev);

	if (FAILED(pProjectile->Ready_GameObject()))
	{
		Safe_Release(pProjectile);
		MSG_BOX("pProjectile Create Failed");
		return nullptr;
	}

	pProjectile->Set_Pos(vPos);
	pProjectile->Set_vecSpeed(vSpeed);
	pProjectile->Set_UseGravity(bUseGravity);

	return pProjectile;
}

void CProjectile::Free()
{
	CGameObject::Free();
}
