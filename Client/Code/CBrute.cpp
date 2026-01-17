#include "pch.h"
#include "CBrute.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN1_AI.h"

CBrute::CBrute(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_ePreState(BRUTE_END),
	m_eCurState(BRUTE_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CBrute::CBrute(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_ePreState(BRUTE_END),
	m_eCurState(BRUTE_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_fGroundY(0.f)
{
}


CBrute::CBrute(const CBrute& rhs)
	: CGameObject(rhs),
	m_ePreState(BRUTE_END),
	m_eCurState(BRUTE_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_fGroundY(rhs.m_fGroundY)
{
}

CBrute::~CBrute()
{
}

HRESULT CBrute::Ready_GameObject()
{
	m_eOBJID = OID_NPC;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	return S_OK;
}

_int CBrute::Update_GameObject(const _float& fTimeDelta)
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

void CBrute::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBrute::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CBrute::OnCollision(CGameObject* pObject)
{
}

HRESULT CBrute::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BruteTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CBrute::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = 17.f;
	m_fGroundY = -2.5f + fScale * 0.5f - 2.5f;
	m_iHp = 10;
	m_fAcmlTime = 0.f;

	// Transform 세팅
	m_pTransformCom->Set_Pos(10.f, m_fGroundY, 80.f);
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_MONSTER);

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

void CBrute::Ready_Event()
{
}

void CBrute::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case BRUTE_IDLE:
	{
		m_fFrameEnd = 32.f;
	}
	break;

	case BRUTE_RUN:
	{
		m_fFrameEnd = 20.f;
	}
	break;

	case BRUTE_JEER:
	{
		m_fFrameEnd = 25.f;
	}
	break;

	case BRUTE_EXECUTE1:
	{
		m_fFrameEnd = 64.f;
	}
	break;

	case BRUTE_EXECUTE2:
	{
		m_fFrameEnd = 33.f;
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CBrute::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;
	m_fAcmlTime += fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case BRUTE_JEER:
			m_eCurState = BRUTE_IDLE;
			break;

		case BRUTE_EXECUTE1:
			m_eCurState = BRUTE_EXECUTE2;
			break;

		case BRUTE_EXECUTE2:
			m_eCurState = BRUTE_IDLE;
			break;
		}
	}
}

void CBrute::Set_Texture()
{
	//_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 8;
	_uint iV = iFrame / 8;

	m_matTex._11 = 0.125f;	// 가로는 8칸 고정
	m_matTex._22 = 0.125f;	// 세로는 8칸 고정(Ratau)

	switch (m_eCurState)
	{
	case BRUTE_IDLE:
		break;

	case BRUTE_RUN:
		break;

	case BRUTE_JEER:
		break;

	case BRUTE_EXECUTE1:
		break;

	case BRUTE_EXECUTE2:
		break;
	}

	//if (bFilpX)
	//{
	//	m_matTex._11 *= -1.f;
	//	m_matTex._31 = _float(iU + 1) * 0.125f;	// 반전 O : 오른쪽에서 왼쪽으로 읽음
	//}
	//else
	//{
	m_matTex._31 = _float(iU) * 0.125f;	// 반전 X : 왼쪽에서 오른쪽으로 읽음
	//}

	m_matTex._32 = _float(iV) * 0.125f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(_uint(m_eCurState));
}

CBrute* CBrute::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos)
{
	CBrute* pBrute = new CBrute(pGraphicDev, StageChannel);

	if (FAILED(pBrute->Ready_GameObject()))
	{
		Safe_Release(pBrute);
		MSG_BOX("pBrute Create Failed");
		return nullptr;
	}

	pBrute->m_pTransformCom->Set_Pos(vPos.x, pBrute->m_fGroundY, vPos.z);

	return pBrute;
}

void CBrute::Free()
{
	CGameObject::Free();
}
