#include "pch.h"
#include "CFollower.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN1_AI.h"

CFollower::CFollower(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_ePreState(FOLLOWER_END),
	m_eCurState(FOLLOWER_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CFollower::CFollower(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_ePreState(FOLLOWER_END),
	m_eCurState(FOLLOWER_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}


CFollower::CFollower(const CFollower& rhs)
	: CGameObject(rhs),
	m_ePreState(FOLLOWER_END),
	m_eCurState(FOLLOWER_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CFollower::~CFollower()
{
}

HRESULT CFollower::Ready_GameObject()
{
	m_eOBJID = OID_NPC;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	return S_OK;
}

_int CFollower::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);
	// 충돌체 디버그용
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		return iExit;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CFollower::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CFollower::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CFollower::OnCollision(CGameObject* pObject)
{
	if (pObject->Get_OBJID() == OID_BORDER)
	{
		_vec3 vCurPos;
		m_pTransformCom->Get_Info(INFO_POS, &vCurPos);

		Engine::CCollider* pBorderCol = dynamic_cast<Engine::CCollider*>(
			pObject->Get_Component(ID_STATIC, L"Com_Collider"));

		if (nullptr == pBorderCol)
			return;

		const Engine::AABB& borderAABB = pBorderCol->Get_AABB();

		const _float fPlayerHalf = 0.5f;

		_float fOverlapX = (borderAABB.hx + fPlayerHalf) - abs(vCurPos.x - borderAABB.x);
		_float fOverlapZ = (borderAABB.hz + fPlayerHalf) - abs(vCurPos.z - borderAABB.z);

		if (fOverlapX > 0.f && fOverlapZ > 0.f)
		{
			if (fOverlapX < fOverlapZ)
			{
				if (vCurPos.x < borderAABB.x)
					vCurPos.x -= fOverlapX + 0.1f;
				else
					vCurPos.x += fOverlapX + 0.1f;
			}
			else
			{
				if (vCurPos.z < borderAABB.z)
					vCurPos.z -= fOverlapZ + 0.1f;
				else
					vCurPos.z += fOverlapZ + 0.1f;
			}

			m_pTransformCom->Set_Pos(vCurPos.x, vCurPos.y, vCurPos.z);

		}

		return;
	}
}

HRESULT CFollower::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TarotSellerTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CFollower::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = 7.f;
	m_fGroundY = -2.5f + fScale * 0.5f;
	m_iHp = 10;

	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 20), m_fGroundY, _float(rand() % 20) + 80.f);
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_MONSTER);

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

void CFollower::Ready_Event()
{
}

void CFollower::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case FOLLOWER_IDLE:
	{
		m_fFrameEnd = 64.f;
	}
	break;

	case FOLLOWER_RUN:
	{
		m_fFrameEnd = 32.f;
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CFollower::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;
		m_eCurState = FOLLOWER_STATE(Get_Rand_Int(1, FOLLOWER_END) - 1);
	}
}

void CFollower::Set_TextureSet()
{
	wstring strPreKey = m_strFrameKey;

	_uint iFrame = _uint(m_fFrame);				// 현재 프레임

	switch (m_eCurState)
	{
	case B2S_IDLE:
		m_strFrameKey = L"BossLeshy_Idle";
		break;

	case B2S_DIG:
		m_strFrameKey = L"BossLeshy_Dig";
		break;

	case B2S_ESCAPE:
		m_strFrameKey = L"BossLeshy_Escape";
		break;

	case B2S_HIT:
		m_strFrameKey = L"BossLeshy_Hit";
		break;

	case B2S_SMASH:
		m_strFrameKey = L"BossLeshy_Smash";
		break;

	case B2S_SHOOT:
		m_strFrameKey = L"BossLeshy_Summon";
		break;

	case B2S_SUMMON:
		m_strFrameKey = L"BossLeshy_Summon";
		break;

	case B2S_SPAWN:
		m_strFrameKey = L"BossLeshy_Spawn";
		break;

	case B2S_DIE:
		m_strFrameKey = L"BossLeshy_Die";
		break;

	case B2S_DEAD:
		m_strFrameKey = L"BossLeshy_Dead";
		break;

	case B2S_SPIKE1:
	case B2S_SPIKE2:
		m_strFrameKey = L"BossLeshy_Spike";
		break;
	}

	if (m_strFrameKey != strPreKey) m_fFrame = 0.f;

	m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(m_strFrameKey);

	//_vec3 vDir = *(m_pAICom->Get_Dir());		// AI로부터 받아온 방향
	//_bool bFlipX = vDir.x > 0.f ? true : false;	// 반전 여부
	//
	//if (bFlipX)
	//{
	//	m_matTex._11 *= -1.f;
	//	m_matTex._31 = 1.f;	// 반전 O : 오른쪽에서 왼쪽으로 읽음
	//}
	//
	//m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);
	//
	//D3DXMatrixIdentity(&m_matTex);

	m_pTexSetCom->Set_Texture(m_strFrameKey, _uint(m_fFrame));
}

CFollower* CFollower::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
	CFollower* pTarotSeller = new CFollower(pGraphicDev, StageChannel);

	if (FAILED(pTarotSeller->Ready_GameObject()))
	{
		Safe_Release(pTarotSeller);
		MSG_BOX("pTarotSeller Create Failed");
		return nullptr;
	}

	return pTarotSeller;
}

void CFollower::Free()
{
	CGameObject::Free();
}
