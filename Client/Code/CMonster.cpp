#include "pch.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}


CMonster::CMonster(const CMonster& rhs)
	: CGameObject(rhs), m_vPos(rhs.m_vPos)
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
	
	m_hmapSubHandles.insert({ L"Monster_Rotate", m_pMessageChannel->Subscribe(L"Monster.Move", [this](const IMessageChannel::EVENT& Event) {
		if (Event.eOBJID == this->Get_OBJID()) {
			m_pTransformCom->Rotation(ROT_Y, 1.f);
		}
		}) });

	m_hmapSubHandles.insert({ L"Monster_Damaged", m_pMessageChannel->Subscribe(L"Monster.Attacked", [this](const IMessageChannel::EVENT& Event) {
	if (Event.eOBJID == this->Get_OBJID()) {
		for (auto& Target : any_cast<vector<CGameObject*>>(Event.hmapData.find(L"Target")->second))
		{
			if (Target == this)
			{
				m_iHp -= any_cast<_int>(Event.hmapData.find(L"Attack")->second);
			}
		}
	}
	}) });

	return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	m_pColliderCom->UpdateFromTransform(m_pTransformCom);
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

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
