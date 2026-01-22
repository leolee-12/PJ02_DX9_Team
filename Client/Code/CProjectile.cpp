#include "pch.h"
#include "CProjectile.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CEffectMgr.h"
#include "CTrailEffect.h"

CProjectile::CProjectile(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CGameObject(pGraphicDev),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_iAttack(0),
		m_fGroundY(0.f),
		m_fAcmlTime(0.f),
		m_fLifeTime(0.f),
		m_fGravity(0.f),
		m_bUseGravity(false),
		m_pTrailEffect(nullptr)
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
		m_fAcmlTime(rhs.m_fAcmlTime),
		m_fLifeTime(rhs.m_fLifeTime),
		m_fGravity(rhs.m_fGravity),
		m_bUseGravity(rhs.m_bUseGravity),
		m_pTrailEffect(nullptr)
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
	m_eOBJID = OID_PROJECTILE;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	return S_OK;
}

_int CProjectile::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bUseGravity) m_vSpeed.y += m_fGravity * fTimeDelta;
	m_fAcmlTime += fTimeDelta;

	m_pTransformCom->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);

	Move_Frame(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (m_fAcmlTime >= m_fLifeTime)
	{
		m_pColliderCom->UnregisterFromManager();
		return DEAD;
	}

	if (m_pTrailEffect)
		m_pTrailEffect->Update_OwnerData(m_vPos, m_vSpeed);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CProjectile::LateUpdate_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	_float fHalfScale = m_pTransformCom->Get_Scale(ROT_X) * 0.4f;
	AABB tAABB = { m_vPos.x, m_vPos.y, m_vPos.z, fHalfScale, fHalfScale, fHalfScale };
	m_pColliderCom->Set_AABB(tAABB);
	m_pColliderCom->UpdateFromCustom(tAABB);

	if (g_bDebug) m_pColliderCom->Update_AABBforRender();

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CProjectile::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(0);

	Set_Material();

	m_pBufferCom->Render_Buffer();

	Reset_Material();
}

void CProjectile::OnCollision(CGameObject* pObject)
{
	if (m_iHp == 0) return;

	if (pObject->Get_OBJID() == OID_PLAYER || pObject->Get_OBJID() == OID_MONSTER)
	{
		m_iHp = 0;
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
	_float fScale = 1.5f;
	m_fGroundY = -2.5f + fScale * 0.5f;
	m_iAttack = 1;
	m_iHp = 1;
	m_fAcmlTime = 0.f;
	m_fLifeTime = 10.f;
	m_fGravity = -9.8f * 2.f;

	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 20), m_fGroundY, _float(rand() % 20));
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);
	m_pTransformCom->Update_Component(0.f);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, m_eColGroup);

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

void CProjectile::Set_Material()
{
	// 텍스처 색상 혼합
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_LERP);	// 색상 : 선형 보간
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG0, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE);	// Arg0 : 텍스처의 알파
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);	// Arg1 : Arg0이 1일 때 색상 = 텍스처 원본
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);	// Arg2 : Arg0이 0일 때 색상 = TFACTOR
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_tColor);	// TFACTOR 설정 (가장자리 색상)

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE4X);	// 알파 : x2
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// Arg1 : 텍스처의 알파

	m_bMtrl = true;
}

void CProjectile::Reset_Material()
{
	if (!m_bMtrl) return;

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

	m_bMtrl = false;
}

CProjectile* CProjectile::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vSpeed, _bool bUseGravity, COLGROUP eGroup, D3DXCOLOR tColor)
{
	CProjectile* pProjectile = new CProjectile(pGraphicDev);

	pProjectile->m_eColGroup = eGroup;

	if (FAILED(pProjectile->Ready_GameObject()))
	{
		Safe_Release(pProjectile);
		MSG_BOX("pProjectile Create Failed");
		return nullptr;
	}

	pProjectile->Set_Pos(vPos);
	pProjectile->Set_vecSpeed(vSpeed);
	pProjectile->Set_UseGravity(bUseGravity);
	pProjectile->m_tColor = tColor;

	return pProjectile;
}

void CProjectile::Free()
{
	if (m_pTrailEffect)
	{
		m_pTrailEffect->Set_Dead();  // CEffectMgr가 정리
		m_pTrailEffect = nullptr;
	}

	CGameObject::Free();
}
