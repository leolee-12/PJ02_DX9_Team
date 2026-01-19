#include "pch.h"
#include "CNode.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"

CNode::CNode(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_eUserID(USERID_END)
{
}

CNode::CNode(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_eUserID(USERID_END)
{
}


CNode::CNode(const CNode& rhs)
	: CGameObject(rhs),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_eUserID(rhs.m_eUserID)
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

	Set_Texture();

	Set_Material();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	Reset_Material();
}

void CNode::Set_NodeScale(const _vec3& vScale)
{
	m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);
}

void CNode::Set_NodePos(const _vec3& vPos)
{
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CNode::Set_Material()
{
	if (!m_bUseMtrl) return;

	switch (m_eUserID)
	{
	case MONSTER_B1:
	{
		_float fMax = 1.f;
		_float fRatio = min(m_fAcmlTime / 2.f, 1.f);

		// 텍스처 색상 혼합
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

		DWORD dwCol = DWORD(255 * fRatio * fMax);
		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, dwCol, DWORD(dwCol * 0.1f), DWORD(dwCol * 0.1f)));
	}
	break;

	default:
		break;
	}
	
	m_bMtrl = true;
}

void CNode::Reset_Material()
{
	if (!m_bMtrl) return;

	switch (m_eUserID)
	{
	case MONSTER_B1:
	{
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	break;

	default:
		break;
	}

	m_bMtrl = false;
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(m_strProtoTexKey));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CNode::Ready_Variable()
{
	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 20), 1.f, _float(rand() % 20));
	m_pTransformCom->Set_Scale(2.f, 2.f, 2.f);

	// Anim 관련 세팅
	m_fFrameEnd = 16.f;
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

void CNode::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;
	m_fAcmlTime += fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;
	}
}

void CNode::Set_Texture()
{
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 1.f;		// 세로는 1칸 고정(Node)
	m_matTex._31 = _float(iU) * 0.0625f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(0);
}

CNode* CNode::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, const _tchar* pProtoTexKey)
{
	CNode* pNode = new CNode(pGraphicDev, StageChannel);

	pNode->m_strProtoTexKey = pProtoTexKey;	// Ready에 필요

	if (FAILED(pNode->Ready_GameObject()))
	{
		Safe_Release(pNode);
		MSG_BOX("pNode Create Failed");
		return nullptr;
	}

	return pNode;
}

void CNode::Free()
{
	CGameObject::Free();
}
