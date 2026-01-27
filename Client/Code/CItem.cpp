#include "pch.h"
#include "CItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CActiveItem.h"
#include "CPassiveItem.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_ePreState(IS_END),
	m_eCurState(IS_SPAWN),
	m_fAccTime(0.f),
	m_fGravity(0.f),
	m_fBounceDamp(0.f),
	m_fGroundY(0.f)
{
}

CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_ePreState(IS_END),
	m_eCurState(IS_SPAWN),
	m_fAccTime(0.f),
	m_fGravity(0.f),
	m_fBounceDamp(0.f),
	m_fGroundY(0.f)
{
}

CItem::CItem(const CItem& rhs)
	: CGameObject(rhs),
	m_ePreState(rhs.m_ePreState),
	m_eCurState(rhs.m_eCurState),
	m_fAccTime(rhs.m_fAccTime),
	m_fGravity(rhs.m_fGravity),
	m_fBounceDamp(rhs.m_fBounceDamp),
	m_fGroundY(rhs.m_fGroundY)
{
}

CItem::~CItem()
{
}

HRESULT CItem::Ready_GameObject()
{
	m_eOBJID = OID_ITEM;
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();

	Ready_Event();

	return S_OK;
}

void CItem::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_State();
	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CItem::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(_uint(m_eItemID));

	m_pBufferCom->Render_Buffer();
}

HRESULT CItem::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ItemTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CItem::Ready_Variable()
{
	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_ITEM);
	m_pTransformCom->Set_Scale(2.f, 2.f, 1.f);

	// 게임로직 변수 세팅

	_float fX = (rand() % 20 - 10.f) * 0.3f * m_fThrowRange;
	_float fZ = (rand() % 20 - 10.f) * 0.3f * m_fThrowRange;
	m_vSpeed = { fX, 10.f + Get_Rand_Float(-2.5f, 2.5f), fZ};
	m_fGravity = -9.8f;
	m_fBounceDamp = 0.4f + Get_Rand_Float(-0.1f, 0.1f);
	m_fGroundY = -1.5f;
}

void CItem::Ready_Event()
{
}

void CItem::Check_State()
{
	if (m_ePreState == m_eCurState)
		return;

	switch (m_eCurState)
	{
	case IS_SPAWN:
		break;
	case IS_IDLE:
		m_vSpeed = { 0.f, 0.f, 0.f };
		m_fAccTime = 0.f;
		break;
	case IS_CHASE:
		break;
	case IS_SUMMON:
		break;
	}

	m_ePreState = m_eCurState;
}

void CItem::Update_Spawn(const _float& fTimeDelta)
{
	m_vSpeed.y += m_fGravity * 3.f * fTimeDelta;
	m_pTransformCom->Move_Pos(&m_vSpeed, fTimeDelta, 1.f);

	_vec3 vPos;

	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	if ((vPos.y <= m_fGroundY) && (m_vSpeed.y < 0.f))
	{
		m_pTransformCom->Set_Pos(vPos.x, m_fGroundY, vPos.z);
		m_vSpeed.y = -m_vSpeed.y * m_fBounceDamp;
		m_vSpeed.x = m_vSpeed.x * m_fBounceDamp;

		if (fabsf(m_vSpeed.y) < 1.f)
			m_eCurState = IS_IDLE;
	}
}

CItem* CItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, const _vec3& vPos, ITEMID eID, _bool isActive, _float fThrowRange)
{
	if (eID < IG_GOLD || ID_END <= eID)
	{
		MSG_BOX("CItem Create Failed : ItemID out of Range");
		return nullptr;
	}

	CItem* pItem = nullptr;

	if (isActive)	pItem = CActiveItem::Create(pGraphicDev, StageChannel, fThrowRange);
	else			pItem = CPassiveItem::Create(pGraphicDev, StageChannel, eID, fThrowRange);

	NULL_CHECK_RETURN(pItem, nullptr);

	pItem->m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	pItem->m_eItemID = eID;

	return pItem;
}

void CItem::Free()
{
	CGameObject::Free();
}
