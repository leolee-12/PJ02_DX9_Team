#include "pch.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include <CItem.h>

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CGameObject(pGraphicDev),
		m_fAttack(0),
		m_fGroundY(0.f)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	:	CGameObject(pGraphicDev, StageChannel),
		m_fAttack(0),
		m_fGroundY(0.f)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}


CMonster::CMonster(const CMonster& rhs)
	:	CGameObject(rhs),
		m_vPos(rhs.m_vPos),
		m_fAttack(rhs.m_fAttack),
		m_fGroundY(rhs.m_fGroundY)
{
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
	m_eOBJID = OID_MONSTER;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pColliderCom->RegisterToManager(this, CL_MONSTER);
	
	m_hmapSubHandles.insert({ L"Monster_Rotate", m_pMessageChannel->Subscribe(L"Monster.Move", [this](const IMessageChannel::EVENT& Event)
		{
			if (Event.eOBJID == this->Get_OBJID())
			{
				m_pTransformCom->Rotation(ROT_Y, 1.f);
			}
		}
	) });

	m_hmapSubHandles.insert({ L"Monster_Damaged", m_pMessageChannel->Subscribe(L"Monster.Attacked", [this](const IMessageChannel::EVENT& Event)
		{
			for (auto& Target : any_cast<vector<CGameObject*>>(Event.hmapData.find(L"Target")->second))
			{
				if (Target == this)
				{
					m_iHp -= any_cast<_int>(Event.hmapData.find(L"Attack")->second);
				}
			}
		}
	) });

	return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	m_pColliderCom->UpdateFromTransform(m_pTransformCom);
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		return iExit;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);

	/*Engine::CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()->
		Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

	if (nullptr == pPlayerTransformCom)
		return;

	_vec3 vPlayerPos{};
	pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

	m_pTransformCom->Chase_Target(&vPlayerPos, fTimeDelta, 5.f);*/

}

void CMonster::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CMonster::OnCollision(CGameObject* pObject)
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

HRESULT CMonster::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MonsterTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CMonster::Create_Item()
{
	_uint iItemCount = Get_Rand_Int(3, 5);
	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);

	for (_uint i = 0; i < iItemCount; ++i)
	{
		CGameObject* pItem;
		_float fY(vPos.y - m_pTransformCom->Get_Scale(ROT_Y) * 0.25f);
		pItem = CItem::Create(m_pGraphicDev, m_pMessageChannel, _vec3(vPos.x, fY, vPos.z), CItem::IG_PASSION, true);

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

CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
	CMonster* pMonster = new CMonster(pGraphicDev, StageChannel);

	if (FAILED(pMonster->Ready_GameObject()))
	{
		Safe_Release(pMonster);
		MSG_BOX("pMonster Create Failed");
		return nullptr;
	}

	return pMonster;
}

void CMonster::Free()
{
	CGameObject::Free();
}
