#include "pch.h"
#include "CTarotSeller.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN1_AI.h"

CTarotSeller::CTarotSeller(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_ePreState(TAROT_END),
	m_eCurState(TAROT_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CTarotSeller::CTarotSeller(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_ePreState(TAROT_END),
	m_eCurState(TAROT_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}


CTarotSeller::CTarotSeller(const CTarotSeller& rhs)
	: CGameObject(rhs),
	m_ePreState(TAROT_END),
	m_eCurState(TAROT_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CTarotSeller::~CTarotSeller()
{
}

HRESULT CTarotSeller::Ready_GameObject()
{
	m_eOBJID = OID_NPC;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	return S_OK;
}

_int CTarotSeller::Update_GameObject(const _float& fTimeDelta)
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

void CTarotSeller::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTarotSeller::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CTarotSeller::OnCollision(CGameObject* pObject)
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

HRESULT CTarotSeller::Add_Component()
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

void CTarotSeller::Ready_Variable()
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

void CTarotSeller::Ready_Event()
{
}

void CTarotSeller::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case TAROT_IDLE:
	{
		m_fFrameEnd = 256.f;
	}
	break;

	case TAROT_TALK:
	{
		m_fFrameEnd = 128.f;
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CTarotSeller::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case TAROT_IDLE:
			m_eCurState = TAROT_TALK;
			break;

		case TAROT_TALK:
			m_eCurState = TAROT_IDLE;
			break;
		}
	}
}

void CTarotSeller::Set_Texture()
{
	//_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.0625f;	// 세로는 16칸 고정(TarotSeller)

	switch (m_eCurState)
	{
	case TAROT_IDLE:
		break;

	case TAROT_TALK:
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

	m_matTex._32 = _float(iV) * 0.0625f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(_uint(m_eCurState));
}

CTarotSeller* CTarotSeller::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos)
{
	CTarotSeller* pTarotSeller = new CTarotSeller(pGraphicDev, StageChannel);

	if (FAILED(pTarotSeller->Ready_GameObject()))
	{
		Safe_Release(pTarotSeller);
		MSG_BOX("pTarotSeller Create Failed");
		return nullptr;
	}

	pTarotSeller->m_pTransformCom->Set_Pos(vPos.x, pTarotSeller->m_fGroundY, vPos.z);

	return pTarotSeller;
}

void CTarotSeller::Free()
{
	CGameObject::Free();
}
