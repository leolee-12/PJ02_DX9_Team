#include "pch.h"
#include "CBreakableTree.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CGrassBuffer.h"
#include "CCollider.h"
#include "CInteractMgr.h"
#include "CFontMgr.h"
#include "CResourceWorkBar.h"

CBreakableTree::CBreakableTree(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_iTextureIndex(0)
	, m_fScale(1.f)
	, m_fBaseScale(1.f)
	, m_fWorkGauge(0.f)
	, m_fPreWorkGauge(0.f)
{
}

CBreakableTree::CBreakableTree(const CBreakableTree& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_iTextureIndex(rhs.m_iTextureIndex)
	, m_fScale(rhs.m_fScale)
	, m_fBaseScale(rhs.m_fBaseScale)
	, m_fWorkGauge(0.f)
	, m_fPreWorkGauge(0.f)
{
}

CBreakableTree::~CBreakableTree()
{
}

HRESULT CBreakableTree::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fFrame = 0.f;
	m_fFrameSpeed = 24.f;
	m_fFrameEnd = 32.f;

	m_pColliderCom->RegisterToManager(this, CL_GRASS);
	CInteractMgr::GetInstance()->Register_IObj(CInteractMgr::WOOD, this);

	m_hmapSubHandles.insert({ L"Monster_Damaged", m_pMessageChannel->Subscribe(L"Monster.Attacked", [this](const IMessageChannel::EVENT& Event) {
		for (auto& Target : any_cast<vector<CGameObject*>>(Event.hmapData.find(L"Target")->second))
		{
			if (Target == this)
			{
				this->m_iHp = 0;
			}
		}
	}) });

	m_pWorkBar = CResourceWorkBar::Create(m_pGraphicDev, _float(m_iHp), _vec3{});
	m_pWorkBar->UnActive();

	return S_OK;
}

_int CBreakableTree::Update_GameObject(const _float& fTimeDelta)
{
	if (g_bDebug) { m_pColliderCom->Update_AABBforRender(); }

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	Update_WorkBar(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		CInteractMgr::GetInstance()->Unregister_IObj(CInteractMgr::WOOD, this);
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CBreakableTree::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_fWorkGauge - m_fPreWorkGauge < 0.0001f)	m_iTextureIndex = 0;	// IDLE
	else
	{
		m_pWorkBar->Active();
		m_iTextureIndex = 1;	// HIT
	}

	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Compute_Bilboard(BBD_X);
	Compute_ViewDepth(&vPos);

	Check_Status();

	m_pWorkBar->LateUpdate_GameObject(fTimeDelta);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBreakableTree::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CBreakableTree::Set_ObjectData(const Engine::OBJECTDATA& objData)
{
	m_iTextureIndex = objData.textureIndex;
	m_fScale = objData.scale;
	m_fBaseScale = objData.scale;

	m_pTransformCom->Set_Pos(objData.x, objData.y, objData.z);
	m_pTransformCom->Set_Scale(m_fScale, m_fScale, m_fScale);
}

void CBreakableTree::Check_Status()
{
	if (Is_WorkComplete()) m_iHp = 0;

	m_fPreWorkGauge = m_fWorkGauge;
}

void CBreakableTree::OnCollision(CGameObject* pObject)
{
}

HRESULT CBreakableTree::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// GrassBuffer (dynamic vertex buffer for sway effect)
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Grass Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BTreeTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));
	//static_cast<Engine::CCollider*>(pComponent)->Set_AABB();

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CBreakableTree::Set_Texture()
{
	//_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 2;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.5f;	// 세로는 2칸 고정(BreakableTree)
	m_matTex._31 = _float(iU) * 0.0625f;	// 반전 X : 왼쪽에서 오른쪽으로 읽음
	m_matTex._32 = _float(iV) * 0.5f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(m_iTextureIndex);
}

void CBreakableTree::Update_WorkBar(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	vPos.y += 3.f;
	m_pWorkBar->Set_TargetPos(vPos);
	m_pWorkBar->Update_CurWork(m_fWorkGauge);
	m_pWorkBar->Update_GameObject(fTimeDelta);
}

CBreakableTree* CBreakableTree::Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::OBJECTDATA& objData, IMessageChannel* pMessageChannel)
{
	CBreakableTree* pBreakableTree = new CBreakableTree(pGraphicDev);

	pBreakableTree->m_pMessageChannel = pMessageChannel;
	pBreakableTree->m_pMessageChannel->AddRef();

	if (FAILED(pBreakableTree->Ready_GameObject()))
	{
		Safe_Release(pBreakableTree);
		MSG_BOX("pBreakableTree Create Failed");
		return nullptr;
	}

	pBreakableTree->Set_ObjectData(objData);
	pBreakableTree->m_pTransformCom->Update_Component(0.f);

	return pBreakableTree;
}

void CBreakableTree::Free()
{
	Safe_Release(m_pWorkBar);
	CGameObject::Free();
}
