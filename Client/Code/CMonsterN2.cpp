#include "pch.h"
#include "CMonsterN2.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN2_AI.h"

CMonsterN2::CMonsterN2(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CMonster(pGraphicDev),
		m_ePreState(N2S_END),
		m_eCurState(N2S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_iAttack(0),
		m_fNodeFrameEnd(0.f),
		m_fWaveTime(0.f)
{
	ZeroMemory(m_vNodePos, sizeof(m_vNodePos));
}

CMonsterN2::CMonsterN2(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	:	CMonster(pGraphicDev, StageChannel),
		m_ePreState(N2S_END),
		m_eCurState(N2S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_iAttack(0),
		m_fNodeFrameEnd(0.f),
		m_fWaveTime(0.f)
{
	ZeroMemory(m_vNodePos, sizeof(m_vNodePos));
}


CMonsterN2::CMonsterN2(const CMonsterN2& rhs)
	:	CMonster(rhs),
		m_ePreState(N2S_END),
		m_eCurState(N2S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_iAttack(rhs.m_iAttack),
		m_fNodeFrameEnd(rhs.m_fNodeFrameEnd),
		m_fWaveTime(0.f)
{
	memcpy(m_vNodePos, rhs.m_vNodePos, sizeof(m_vNodePos));
}

CMonsterN2::~CMonsterN2()
{
}

HRESULT CMonsterN2::Ready_GameObject()
{
	m_eOBJID = OID_MONSTER;

	if (FAILED(Add_Component()))
		return E_FAIL;
	
	Ready_Variable();

	Ready_Event();

	return S_OK;
}

_int CMonsterN2::Update_GameObject(const _float& fTimeDelta)
{
	m_fWaveTime += fTimeDelta;

	Move_Frame(fTimeDelta);

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

void CMonsterN2::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Update_State();

	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMonsterN2::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	
	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

HRESULT CMonsterN2::Add_Component()
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

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	// AI
	pComponent = m_pAICom = dynamic_cast<CN2_AI*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_N2_AI"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_AI", pComponent });

	return S_OK;
}

void CMonsterN2::Ready_Variable()
{
	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 20), 1.f, _float(rand() % 20));
	m_pTransformCom->Set_Scale(3.f, 3.f, 3.f);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_MONSTER);

	// AI 세팅
	m_pAICom->Set_OwnerTransform(m_pTransformCom);
	m_pAICom->Set_TargetTransform(CPersistentMgr::GetInstance()->Get_PlayerTransform());
	m_pAICom->Set_State<MONSTER_N2_STATE>(N2S_SPAWN);

	// Anim 관련 세팅
	m_fNodeFrameEnd = 16.f;
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);

	// 게임로직 변수 세팅
	m_iAttack = 1;
	m_iHp = 10;
}

void CMonsterN2::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Monster_Damaged", m_pMessageChannel->Subscribe(L"Monster.Attacked", [this](const IMessageChannel::EVENT& Event) {
	for (auto& Target : any_cast<vector<CGameObject*>>(Event.hmapData.find(L"Target")->second))
	{
		if (Target == this)
		{
			Attacked(any_cast<_int>(Event.hmapData.find(L"Attack")->second));
			break;
		}
	}
	}) });
}

void CMonsterN2::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case N2S_CRAWL:
	{
		m_fFrameEnd = 14.f;
	}
	break;

	case N2S_JUMP:
	{
		m_fFrameEnd = 16.f;
	}
	break;

	case N2S_LAND:
	{
		m_fFrameEnd = 19.f;
	}
	break;

	case N2S_SPAWN:
	{
		m_fFrameEnd = 1.f;
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CMonsterN2::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case N2S_JUMP:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = N2S_LAND;
			break;

		case N2S_LAND:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = N2S_CRAWL;
			break;

		case N2S_SPAWN:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = N2S_CRAWL;
			break;
		}
	}
}

void CMonsterN2::Set_Texture()
{
	_vec3 vDir = *(m_pAICom->Get_Dir());		// AI로부터 받아온 방향
	_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = m_fFrame;					// 현재 프레임
	_uint iTexIdx = _uint(m_eCurState);			// 텍스처 인덱스

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.25f;	// 세로는 4칸 고정(MonsterN2)

	switch (m_eCurState)
	{
	case N2S_CRAWL:
	{
		if (vDir.z > 0.f) iV += 1;
	}
	break;

	case N2S_JUMP:
	{
	}
	break;

	case N2S_LAND:
	{
	}
	break;

	case N2S_SPAWN:
	{
		iTexIdx = 0;
	}
	break;
	}

	if (bFilpX)
	{
		m_matTex._11 *= -1.f;
		m_matTex._31 = _float(iU + 1) * 0.0625f;	// 반전 O : 오른쪽에서 왼쪽으로 읽음
	}
	else
	{
		m_matTex._31 = _float(iU) * 0.0625f;	// 반전 X : 왼쪽에서 오른쪽으로 읽음
	}

	m_matTex._32 = _float(iV) * 0.25f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(iTexIdx);
}

void CMonsterN2::Attack_HitBox()
{
	AABB tAABB = { m_vPos.x, m_vPos.y, m_vPos.z,
					2.f, 1.f, 2.f };

	vector<CGameObject*> tempVec = CCollisionMgr::GetInstance()->Test_AABB(tAABB, CL_PLAYER);

	if (!tempVec.empty())
	{
		IMessageChannel::EVENT EAttack;
		EAttack.strType = L"Player.Attacked";
		EAttack.eOBJID = Engine::OID_PLAYER;
		EAttack.hmapData.emplace(L"Attack", m_iAttack);
		EAttack.hmapData.emplace(L"Target", tempVec);
		m_pMessageChannel->Publish(EAttack);
	}
}

void CMonsterN2::Attacked(const _int& iAttack)
{
	m_iHp -= iAttack;
}

void CMonsterN2::Update_State()
{
	if (m_eCurState == N2S_SPAWN) return;

	m_eCurState = m_pAICom->Get_RecommendState<MONSTER_N2_STATE>();
}

CMonsterN2* CMonsterN2::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
	CMonsterN2* pMonster = new CMonsterN2(pGraphicDev, StageChannel);

	if (FAILED(pMonster->Ready_GameObject()))
	{
		Safe_Release(pMonster);
		MSG_BOX("pMonster Create Failed");
		return nullptr;
	}

	return pMonster;
}

void CMonsterN2::Free()
{
	CGameObject::Free();
}
