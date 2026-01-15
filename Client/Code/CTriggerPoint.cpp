#include "pch.h"
#include "CTriggerPoint.h"
#include "CCollider.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CMapWarp.h"

CTriggerPoint::CTriggerPoint(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_eTID(Trigger::TI_END)
{

}

CTriggerPoint::CTriggerPoint(const CTriggerPoint& rhs)
	: CGameObject(rhs)
	, m_pTransformCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_eTID(rhs.m_eTID)
{

}

CTriggerPoint::~CTriggerPoint()
{
}

HRESULT CTriggerPoint::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eOBJID = OID_TRIGGER;

	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	m_pColliderCom->Set_AABB(AABB(m_vPos, m_vColHalfSize));
	m_pColliderCom->RegisterToManager(this, CL_TRIGGER);

	return S_OK;
}

_int CTriggerPoint::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	if (g_bDebug) { m_pColliderCom->Update_AABBforRender(); }

	return iExit;
}

void CTriggerPoint::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTriggerPoint::Render_GameObject()
{
}

void CTriggerPoint::OnCollision(CGameObject* pObject)
{
}

HRESULT CTriggerPoint::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });


	return S_OK;
}

void CTriggerPoint::Activate()
{
	IMessageChannel::EVENT TriggerEvent;
	TriggerEvent.strType = L"Trigger.Activate";
	TriggerEvent.hmapData[L"Trigger_TID"] = m_eTID;
	TriggerEvent.hmapData[L"Trigger_Name"] = m_strTriggerName;
	m_pMessageChannel->Publish(TriggerEvent);

	if (m_eTID == Trigger::TI_STAGING)
		m_iHp = 0;
}

CTriggerPoint* CTriggerPoint::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, _vec3 vPos, _vec3 vHalfSize, Trigger::TRIGGERID eTID, const wstring& strTriggerName)
{
	CTriggerPoint* pTrigger = new CTriggerPoint(pGraphicDev);

	pTrigger->m_pMessageChannel = pMessageChannel;
	pTrigger->m_pMessageChannel->AddRef();

	pTrigger->m_vPos = vPos;
	pTrigger->m_vColHalfSize = vHalfSize;
	pTrigger->m_eTID = eTID;
	pTrigger->m_strTriggerName = strTriggerName;

	if (FAILED(pTrigger->Ready_GameObject()))
	{
		Safe_Release(pTrigger);
		MSG_BOX("pTrigger Create Failed");
		return nullptr;
	}

	return pTrigger;
}

void CTriggerPoint::Free()
{
	CGameObject::Free();
}
