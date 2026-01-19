#include "pch.h"
#include "CBreakableTree.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CGrassBuffer.h"
#include "CCollider.h"
#include "CInteractMgr.h"
#include "CFontMgr.h"

CBreakableTree::CBreakableTree(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pGrassBuffer(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_iTextureIndex(0)
	, m_fScale(1.f)
	, m_fBaseScale(1.f)
	, m_fPhase(0.f)
	, m_fWindSpeed(3.f)
	, m_fWindStrength(0.1f)
	, m_fAccTime(0.f)
	, m_fReactStrength(0.f)
	, m_vReactDir(0.f, 0.f, 0.f)
	, m_fWorkGauge(0.f)
{
}

CBreakableTree::CBreakableTree(const CBreakableTree& rhs)
	: CGameObject(rhs)
	, m_pGrassBuffer(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_iTextureIndex(rhs.m_iTextureIndex)
	, m_fScale(rhs.m_fScale)
	, m_fBaseScale(rhs.m_fBaseScale)
	, m_fPhase(0.f)
	, m_fWindSpeed(3.f)
	, m_fWindStrength(0.1f)
	, m_fAccTime(0.f)
	, m_fReactStrength(0.f)
	, m_vReactDir(0.f, 0.f, 0.f)
	, m_fWorkGauge(0.f)
{
}

CBreakableTree::~CBreakableTree()
{
}

HRESULT CBreakableTree::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

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

	return S_OK;
}

_int CBreakableTree::Update_GameObject(const _float& fTimeDelta)
{
	if (g_bDebug) { m_pColliderCom->Update_AABBforRender(); }

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	Check_Status();

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		CInteractMgr::GetInstance()->Unregister_IObj(CInteractMgr::WOOD, this);
	}


	m_fAccTime += fTimeDelta;

	// React is now called via OnCollision callback
	Update_VertexSway(fTimeDelta);

	// Decay react strength over time
	m_fReactStrength *= 0.9f;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CBreakableTree::LateUpdate_GameObject(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	m_pTransformCom->Compute_Bilboard(BBD_X);
	Compute_ViewDepth(&vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBreakableTree::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Alpha test (remove fully transparent pixels)
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x10);
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Set_Texture(m_iTextureIndex);
	m_pGrassBuffer->Render_Buffer();

	// Restore
	//m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


	// -------------------------디버그용------------------------------
	_matrix matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	_vec3 vWorldPos, vViewPos, vndcPos, vScreenPos;
	m_pTransformCom->Get_Info(INFO_POS, &vWorldPos);
	D3DXVec3TransformCoord(&vViewPos, &vWorldPos, &matView);
	D3DXVec3TransformCoord(&vndcPos, &vViewPos, &matProj);
	vScreenPos.x = (vndcPos.x * 0.5f + 0.5f) * _float(WINCX);
	vScreenPos.y = (-vndcPos.y * 0.5f + 0.5f) * _float(WINCY);
	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	wchar_t szGauge[16];

	swprintf_s(szGauge, L" Gauge : %.3f", m_fWorkGauge);
	RECT rcPlayer = { vScreenPos.x - 50, vScreenPos.y - 10, vScreenPos.x + 50, vScreenPos.y + 10 };
	CFontMgr::GetInstance()->Render_Font(L"Font_Lapture20", szGauge, rcPlayer, FontColor, DT_RIGHT | DT_BOTTOM);
	// -------------------------디버그용------------------------------
}

void CBreakableTree::Set_ObjectData(const Engine::OBJECTDATA& objData)
{
	m_iTextureIndex = objData.textureIndex;
	m_fScale = objData.scale;
	m_fBaseScale = objData.scale;

	m_pTransformCom->Set_Pos(objData.x, objData.y, objData.z);
	m_pTransformCom->Set_Scale(m_fScale, m_fScale, m_fScale);

	// Position-based phase for individual sway timing
	m_fPhase = fmodf(objData.x * 12.9898f + objData.z * 78.233f, D3DX_PI * 2.f);
}

void CBreakableTree::Check_Status()
{
	if (Is_WorkComplete()) m_iHp = 0;
}

void CBreakableTree::OnCollision(CGameObject* pObject)
{
	Engine::CTransform* pTransform = dynamic_cast<Engine::CTransform*>(
		pObject->Get_Component(ID_DYNAMIC, L"Com_Transform"));

	if (nullptr == pTransform)
		return;

	_vec3 vObjPos;
	pTransform->Get_Info(INFO_POS, &vObjPos);

	React(vObjPos);
}

void CBreakableTree::React(const _vec3& vObjPos)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	_vec3 vDist = vObjPos - vPos;
	vDist.y = 0.f;
	_float fDist = D3DXVec3Length(&vDist);

	const _float fReactDistance = 2.f;

	if (fDist < fReactDistance)
	{
		// Push grass away from object
		D3DXVec3Normalize(&m_vReactDir, &vDist);
		m_vReactDir = -m_vReactDir;

		// Distance-based strength (closer = stronger)
		m_fReactStrength = (1.f - fDist / fReactDistance);
	}
}

void CBreakableTree::Update_VertexSway(const _float& fTimeDelta)
{
	if (nullptr == m_pGrassBuffer)
		return;

	// 1. Base wind sway (always applied)
	_float fWindOffset = sinf(m_fAccTime * m_fWindSpeed + m_fPhase) * m_fWindStrength;

	// 2. Player reaction sway
	_float fReactOffset = m_vReactDir.x * m_fReactStrength * 0.3f;

	// 3. Apply total offset to buffer
	_float fTotalOffset = fWindOffset + fReactOffset;

	m_pGrassBuffer->Set_TopVertexOffset(fTotalOffset);
}

HRESULT CBreakableTree::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// GrassBuffer (dynamic vertex buffer for sway effect)
	pComponent = m_pGrassBuffer = dynamic_cast<Engine::CGrassBuffer*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GrassBuffer"));

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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TreeTexture"));

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
	// m_pBufferCom, m_pTransformCom, m_pTextureCom are in m_mapComponent
	// They will be released by CGameObject::Free()

	CGameObject::Free();
}
