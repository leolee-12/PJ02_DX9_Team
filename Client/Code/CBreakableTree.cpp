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
#include <CItem.h>
#include <CSoundMgr.h>

CBreakableTree::CBreakableTree(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_iTextureIndex(0)
	, m_fScale(1.f)
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
	m_fFrameSpeed = 6.f;
	m_fFrameEnd = 32.f;
	m_fAccTime = 0.f;

	m_eOBJID = OID_BREAK;

	m_pColliderCom->RegisterToManager(this, CL_BREAK);
	CInteractMgr::GetInstance()->Register_IObj(CInteractMgr::WOOD, this);

	m_pWorkBar = CResourceWorkBar::Create(m_pGraphicDev, _float(m_iHp), _vec3{});
	m_pWorkBar->UnActive();

	return S_OK;
}

_int CBreakableTree::Update_GameObject(const _float& fTimeDelta)
{
	if (g_bDebug) { m_pColliderCom->Update_AABBforRender(); }

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	m_fAccTime += fTimeDelta;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	Update_WorkBar(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		CInteractMgr::GetInstance()->Unregister_IObj(CInteractMgr::WOOD, this);
		Create_Item();
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CBreakableTree::LateUpdate_GameObject(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	if (m_fWorkGauge - m_fPreWorkGauge > 0.0001f)
	{
		m_pWorkBar->Active();

		if (m_fAccTime >= 1.f)
		{
			//_uint iChannel = Get_Rand_Int(SOUND_EFFECT1, SOUND_EFFECT10);

			_tchar strSoundName[128] = L"";
			swprintf_s(strSoundName, L"Wood Chop %d.wav", Get_Rand_Int(0, 3));
			CSoundMgr::GetInstance()->PlaySound3D(strSoundName, SOUND_WOOD, 0.5f, vPos);

			m_fAccTime = 0.f;
		}
	}
	else
	{
		if (m_fAccTime >= 3.f)
		{
			m_pWorkBar->UnActive();
			m_fAccTime = 0.f;
		}
	}

	m_pTransformCom->Compute_Bilboard(BBD_X);
	Compute_ViewDepth(&vPos);

	Check_Status();

	m_pWorkBar->LateUpdate_GameObject(fTimeDelta);
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	AABB tAABB = { vPos, _vec3(2.f,3.5f,2.f) };
	m_pColliderCom->Set_AABB(tAABB);
	m_pColliderCom->UpdateFromCustom(tAABB);
	//-------------------------------------------------

	// 충돌체 디버그용
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();
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

_vec3* CBreakableTree::Get_WorkPos(_vec3* pWorkPos) const
{
	m_pTransformCom->Get_Info(INFO_POS, pWorkPos);
	return pWorkPos;
}

void CBreakableTree::Set_ObjectData(const Engine::OBJECTDATA& objData)
{
	m_iTextureIndex = objData.textureIndex;
	m_fScale = objData.scale;

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

void CBreakableTree::Create_Item()
{
	_uint iItemCount = Get_Rand_Int(3, 5);
	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	for (_uint i = 0; i < iItemCount; ++i)
	{
		CGameObject* pItem;
		_float fY(vPos.y - m_pTransformCom->Get_Scale(ROT_Y) * 0.25f);
		pItem = CItem::Create(m_pGraphicDev, m_pMessageChannel, _vec3(vPos.x, fY, vPos.z), CItem::IG_WOOD, true);

		if (pItem)
		{
			wstring strObjTag = L"Item";

			IMessageChannel::EVENT ESummonMonster;
			ESummonMonster.strType = L"Obj.Add";
			ESummonMonster.eOBJID = Engine::OID_ITEM;
			ESummonMonster.hmapData.emplace(L"Obj", pItem);
			ESummonMonster.hmapData.emplace(L"LayerTag", L"GameLogic_Layer");
			ESummonMonster.hmapData.emplace(L"ObjTag", strObjTag);
			m_pMessageChannel->Publish(ESummonMonster);
		}
	}
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
