#include "pch.h"
#include "CNode.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN2_AI.h"

CNode::CNode(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CNode::CNode(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}


CNode::CNode(const CNode& rhs)
	: CGameObject(rhs),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CNode::~CNode()
{
}

HRESULT CNode::Ready_GameObject()
{
	m_eOBJID = OID_MONSTER;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();

	return S_OK;
}

_int CNode::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		return iExit;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CNode::LateUpdate_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CNode::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

HRESULT CNode::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MonsterN2Texture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CNode::Ready_Variable()
{
	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 20), 1.f, _float(rand() % 20));
	m_pTransformCom->Set_Scale(3.f, 3.f, 3.f);

	// Anim 관련 세팅
	m_fFrameEnd = 16.f;
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

void CNode::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;
	}
}

void CNode::Set_Texture()
{
	_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = m_fFrame;					// 현재 프레임
	_uint iTexIdx = _uint(m_eCurState);			// 텍스처 인덱스

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.25f;	// 세로는 4칸 고정(MonsterN2)


	m_matTex._32 = _float(iV) * 0.25f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(iTexIdx);
}

CNode* CNode::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, CTransform* pOwnerTC)
{
	CNode* pNode = new CNode(pGraphicDev, StageChannel);

	if (FAILED(pNode->Ready_GameObject()))
	{
		Safe_Release(pNode);
		MSG_BOX("pNode Create Failed");
		return nullptr;
	}

	pNode->m_pOwnerTC = pOwnerTC;

	return pNode;
}

void CNode::Free()
{
	CGameObject::Free();
}
