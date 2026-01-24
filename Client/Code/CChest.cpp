#include "pch.h"
#include "CChest.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN1_AI.h"
#include "CSpeechBubble.h"
#include "CFontUI.h"
#include "CSoundMgr.h"
#include "CItem.h"

CChest::CChest(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_ePreState(CHEST_END),
	m_eCurState(CHEST_INTRO),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CChest::CChest(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_ePreState(CHEST_END),
	m_eCurState(CHEST_INTRO),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}


CChest::CChest(const CChest& rhs)
	: CGameObject(rhs),
	m_ePreState(CHEST_END),
	m_eCurState(CHEST_INTRO),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CChest::~CChest()
{
}

HRESULT CChest::Ready_GameObject()
{
	m_eOBJID = OID_CHEST;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();


	return S_OK;
}

_int CChest::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		return iExit;
	}

	if (!m_bWait) {
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}

	return iExit;
}

void CChest::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);

	//------스프라이트 높이와 충돌체 위치 맞춤---------
	_float fY(m_vPos.y - m_pTransformCom->Get_Scale(ROT_Y) * 0.25f);
	AABB tAABB = { m_vPos.x, fY, m_vPos.z - 1.f, 1.f, 1.f, 1.f };
	m_pColliderCom->Set_AABB(tAABB);
	m_pColliderCom->UpdateFromCustom(tAABB);
	//-------------------------------------------------

	// 충돌체 디버그용
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();
}

void CChest::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CChest::OnCollision(CGameObject* pObject)
{
	if (m_bWait) { return; }
	switch (pObject->Get_OBJID())
	{
	case OID_PLAYER:
		if (m_eCurState != CHEST_IDLE) { return; }

		m_eCurState = CHEST_OPEN;
		_tchar strSoundName[128] = L"";
		swprintf_s(strSoundName, L"ItemSpawn%d.wav", Get_Rand_Int(1, 4));
		CSoundMgr::GetInstance()->Play(strSoundName, SOUND_EFFECT, 1.f);
		Create_Item();
		break;
	}
}

HRESULT CChest::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ChestTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });



	return S_OK;
}

void CChest::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = 10.f;
	m_fGroundY = -2.5f + fScale * 0.5f - 1.2f;
	m_iHp = 10;

	// Transform 세팅
	m_pTransformCom->Set_Pos(125.f, m_fGroundY, 15.f);
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	m_pColliderCom->RegisterToManager(this, CL_CHEST);

	Check_Frame();

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

void CChest::Ready_Event()
{
}


void CChest::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case CHEST_INTRO:
		m_fFrameEnd = 28.f;
		break;

	case CHEST_IDLE:
		m_fFrameEnd = 1.f;
		break;

	case CHEST_OPEN:
		m_fFrameEnd = 24.f;
		break;

	case CHEST_END:
		break;
	}

	m_ePreState = m_eCurState;
}

void CChest::Move_Frame(const _float& fTimeDelta)
{
	if (m_bWait) { return; }

	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		IMessageChannel::EVENT ChestEvent;

		switch (m_eCurState)
		{
		case CHEST_INTRO:
			m_eCurState = CHEST_IDLE;
			ChestEvent.strType = L"Chest.Done";
			m_pMessageChannel->Publish(ChestEvent);
			break;

		case CHEST_IDLE:
			break;

		case CHEST_OPEN:
			m_fFrame = m_fFrameEnd - 0.001f;
			break;

		case CHEST_END:
			break;
		}
	}
}

void CChest::Set_Texture()
{
	//_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 8;
	_uint iV = iFrame / 8;

	m_matTex._11 = 0.125f;	// 가로는 8칸 고정
	m_matTex._22 = 0.25f;	// 세로는 8칸 고정(Ratau)

	_uint iIndex;

	switch (m_eCurState)
	{
	case CHEST_INTRO:
		iIndex = 0;
		break;

	case CHEST_IDLE:
		iIndex = 1;
		break;

	case CHEST_OPEN:
		iIndex = 1;
		break;

	case CHEST_END:
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

	m_matTex._32 = _float(iV) * 0.25f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(iIndex);
}

void CChest::Create_Item()
{
	for (_uint i = 0; i < m_iItemCount; ++i)
	{
		CGameObject* pItem;
		_float fY(m_vPos.y - m_pTransformCom->Get_Scale(ROT_Y) * 0.25f);
		pItem = CItem::Create(m_pGraphicDev, m_pMessageChannel, _vec3(m_vPos.x, fY, m_vPos.z - 2.f), CItem::ITEMID(Get_Rand_Int(0, 3)), true);

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

CChest* CChest::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos, _uint iItemCount)
{
	CChest* pChest = new CChest(pGraphicDev, StageChannel);

	pChest->m_vPos = vPos;
	pChest->m_iItemCount = iItemCount;

	if (FAILED(pChest->Ready_GameObject()))
	{
		Safe_Release(pChest);
		MSG_BOX("pChest Create Failed");
		return nullptr;
	}

	pChest->m_pTransformCom->Set_Pos(vPos.x, pChest->m_fGroundY, vPos.z);

	return pChest;
}

void CChest::Free()
{
	CGameObject::Free();
}
