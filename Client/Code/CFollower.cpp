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
	m_fFrameSpeed(0.f),
	m_eInteractType(NONE)
{
}

CFollower::CFollower(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_ePreState(FOLLOWER_END),
	m_eCurState(FOLLOWER_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_eInteractType(NONE)
{
}


CFollower::CFollower(const CFollower& rhs)
	: CGameObject(rhs),
	m_ePreState(FOLLOWER_END),
	m_eCurState(FOLLOWER_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_eInteractType(NONE)
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

		const _float fHalf = 1.f;

		_float fOverlapX = (borderAABB.hx + fHalf) - abs(vCurPos.x - borderAABB.x);
		_float fOverlapZ = (borderAABB.hz + fHalf) - abs(vCurPos.z - borderAABB.z);

		if (fOverlapX > 0.f && fOverlapZ > 0.f)
		{
			if (fOverlapX < fOverlapZ)
			{
				// X축 보정
				if (vCurPos.x < borderAABB.x)
					vCurPos.x = borderAABB.x - borderAABB.hx - fHalf - 0.01f;
				else
					vCurPos.x = borderAABB.x + borderAABB.hx + fHalf + 0.01f;
			}
			else if (fOverlapX > fOverlapZ)
			{
				// Z축 보정
				if (vCurPos.z < borderAABB.z)
					vCurPos.z = borderAABB.z - borderAABB.hz - fHalf - 0.01f;
				else
					vCurPos.z = borderAABB.z + borderAABB.hz + fHalf + 0.01f;
			}

			m_pTransformCom->Set_Pos(vCurPos.x, vCurPos.y, vCurPos.z);
			m_pTransformCom->Update_Component(0.f);
			m_pTransformCom->Compute_Bilboard(BBD_X);
			m_vLerpPos = vCurPos;
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(m_strProtoKey));

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
	_float fScale = 9.f;
	m_fGroundY = -2.5f + fScale * 0.5f - 1.8f;
	m_iHp = 10;

	// Transform 세팅
	m_pTransformCom->Set_Pos(Get_Rand_Float(210.f, 220.f), m_fGroundY, Get_Rand_Float(30.f, 40.f));
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_NPC);

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
		m_fFrameEnd = 24.f;
		break;

	case FOLLOWER_RUN:
		m_fFrameEnd = 14.f;
		break;

	case FOLLOWER_DANCE:
		m_fFrameEnd = 47.f;
		break;

	case FOLLOWER_TRANSFORM:
		m_fFrameEnd = 88.f;
		break;

	case FOLLOWER_UNCONVERT:
		m_fFrameEnd = 48.f;
		break;

	case FOLLOWER_CONVERT:
		m_fFrameEnd = 85.f;
		break;

	case FOLLOWER_ACTION:
	{
		m_eInteractType = INTERACT_TYPE(Get_Rand_Int(0, 5));

		switch (m_eInteractType)
		{
		case NONE:	// (IDLE로 전환)
			m_fFrameEnd = 24.f;
			break;

		case WOOD:
			m_fFrameEnd = 98.f;
			break;

		case ROCK:
			m_fFrameEnd = 46.f;
			break;

		case BUILD:
			m_fFrameEnd = 83.f;
			break;

		case FOOD:
			m_fFrameEnd = 40.f;
			break;

		case PRAY:
			m_fFrameEnd = 60.f;
			break;
		}
	}
		break;

	case FOLLOWER_RECRUIT:
		m_iRecruitState = 0;
		m_fFrameEnd = 20.f;
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
		m_eCurState = FOLLOWER_IDLE;
		//if (m_eCurState != FOLLOWER_RECRUIT)
		//	m_eCurState = FOLLOWER_STATE(Get_Rand_Int(1, FOLLOWER_END) - 1);
		//else
		//{
		//	if (m_iRecruitState == 0)
		//	{
		//		m_iRecruitState = 1;
		//		m_fFrameEnd = 96;
		//	}
		//	else if (m_iRecruitState == 1)
		//	{
		//		m_iRecruitState = 2;
		//		m_fFrameEnd = 75;
		//	}
		//	else if (m_iRecruitState == 2)
		//	{
		//		m_eCurState = FOLLOWER_STATE(Get_Rand_Int(1, FOLLOWER_END) - 1);
		//	}
		//}
	}
}

void CFollower::Set_Texture()
{
	//_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임
	_uint iTexIdx = _uint(m_eCurState);
	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.125f;	// 세로는 8칸 고정(Follower)

	switch (m_eCurState)
	{
	case FOLLOWER_IDLE:
		break;

	case FOLLOWER_RUN:
		break;

	case FOLLOWER_DANCE:
		break;

	case FOLLOWER_TRANSFORM:
		break;

	case FOLLOWER_UNCONVERT:
		break;

	case FOLLOWER_CONVERT:
		break;

	case FOLLOWER_ACTION:
	{
		switch(m_eInteractType)
		{
		case NONE:
			iTexIdx = 0;
			break;
		case WOOD:
			iTexIdx = 6;
			break;
		case ROCK:
			iTexIdx = 7;
			break;
		case BUILD:
			iTexIdx = 8;
			break;
		case FOOD:
			iTexIdx = 9;
			break;
		case PRAY:
			iTexIdx = 10;
			break;
		}
	}
		break;

	case FOLLOWER_RECRUIT:
	{
		switch (m_iRecruitState)
		{
		case 0:	// START
			iTexIdx = 11;
			break;
		case 1:	// LOOP
			iTexIdx = 12;
			break;
		case 2:	// END
			iTexIdx = 13;
			break;
		}
	}
		break;
	}

	//if (bFilpX)
	//{
	//	m_matTex._11 *= -1.f;
	//	m_matTex._31 = _float(iU + 1) * 0.125f;	// 반전 O : 오른쪽에서 왼쪽으로 읽음
	//}
	//else
	//{
	m_matTex._31 = _float(iU) * 0.0625f;	// 반전 X : 왼쪽에서 오른쪽으로 읽음
	//}

	m_matTex._32 = _float(iV) * 0.125f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(_uint(m_eCurState));
}


CFollower* CFollower::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, const _tchar* pProtoKey)
{
	CFollower* pFollower = new CFollower(pGraphicDev, StageChannel);

	pFollower->m_strProtoKey = pProtoKey;

	if (FAILED(pFollower->Ready_GameObject()))
	{
		Safe_Release(pFollower);
		MSG_BOX("pFollower Create Failed");
		return nullptr;
	}

	return pFollower;
}

void CFollower::Free()
{
	CGameObject::Free();
}
