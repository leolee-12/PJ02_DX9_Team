#include "pch.h"
#include "CMonsterN2.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CNode.h"
#include "CN2_AI.h"

CMonsterN2::CMonsterN2(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CMonster(pGraphicDev),
		m_ePreState(N2S_END),
		m_eCurState(N2S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_iAttack(0)
{
	ZeroMemory(m_pNode, sizeof(m_pNode));
}

CMonsterN2::CMonsterN2(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	:	CMonster(pGraphicDev, StageChannel),
		m_ePreState(N2S_END),
		m_eCurState(N2S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_iAttack(0)
{
	ZeroMemory(m_pNode, sizeof(m_pNode));
}


CMonsterN2::CMonsterN2(const CMonsterN2& rhs)
	:	CMonster(rhs),
		m_ePreState(N2S_END),
		m_eCurState(N2S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_iAttack(rhs.m_iAttack)
{
	memcpy(m_pNode, rhs.m_pNode, sizeof(m_pNode));
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
	Move_Frame(fTimeDelta);

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		return iExit;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	_vec3 vPrevPos = m_vPos;
	_vec3 vDir = *m_pAICom->Get_Dir();

	for (_uint i = 0; i < 3; ++i)
	{
		_vec3 vNewDir = Compute_LimitedDir(270.f * fTimeDelta, m_pNode[i]->Get_NodeDir(), vPrevPos - m_pNode[i]->Get_NodePos());
		_vec3 vNewPos = vPrevPos - vNewDir * 0.3f;

		if (vDir.z > 0) vNewPos.z -= 0.01f;
		else			vNewPos.z += 0.01f;

		m_pNode[i]->Set_NodePos(vNewPos);
		m_pNode[i]->Set_NodeDir(vNewDir);
		m_pNode[i]->Update_GameObject(fTimeDelta);

		vPrevPos = vNewPos;
	}
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

	for (_uint i = 0; i < 3; ++i)
		m_pNode[i]->LateUpdate_GameObject(fTimeDelta);
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
	m_pTransformCom->Set_Pos(_float(rand() % 10), 1.f, _float(rand() % 10));
	_vec3 vScale{ 3.f, 3.f, 3.f };
	m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_MONSTER);

	// AI 세팅
	m_pAICom->Set_OwnerTransform(m_pTransformCom);
	m_pAICom->Set_TargetTransform(CPersistentMgr::GetInstance()->Get_PlayerTransform());
	m_pAICom->Set_State<MONSTER_N2_STATE>(N2S_SPAWN);

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);

	// 게임로직 변수 세팅
	m_iAttack = 1;
	m_iHp = 10;


	// 마디 세팅
	vScale *= 0.7f;
	m_pNode[0] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_N2Node1Texture");
	m_pNode[0]->Set_NodeScale(vScale);

	vScale *= 0.7f;
	m_pNode[1] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_N2Node2Texture");
	m_pNode[1]->Set_NodeScale(vScale);

	vScale *= 0.7f;
	m_pNode[2] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_N2Node3Texture");
	m_pNode[2]->Set_NodeScale(vScale);
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
		m_fFrameEnd = 16.f;
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
	case N2S_STOP:
	{
		m_fFrameEnd = 16.f;
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
		case N2S_CRAWL:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = N2S_STOP;
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
		if (vDir.z > 0.f) iV += 2;
	}
	break;

	case N2S_JUMP:
	{
		if (vDir.z > 0.f) iV += 2;
	}
	break;

	case N2S_LAND:
	{
		if (vDir.z > 0.f) iV += 2;
	}
	break;

	case N2S_SPAWN:
	{
		iTexIdx = 0;
	}
	break;

	case N2S_STOP:
	{
		iTexIdx = 0;
		if (vDir.z > 0.f) iV += 2;
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

_vec3 CMonsterN2::Compute_LimitedDir(const _float& fMaxAngle, const _vec3& vCurDir, const _vec3& vDesiredDir)
{
	_vec3 v1, v2;
	D3DXVec3Normalize(&v1, &vCurDir);
	D3DXVec3Normalize(&v2, &vDesiredDir);

	_float fDot = max(-1.f, min(1.f, D3DXVec3Dot(&v1, &v2)));
	_float fRad = acosf(fDot);
	_float fMaxRad = D3DXToRadian(fMaxAngle);

	if (fRad <= fMaxRad)
		return v2;	// 최대 회전 각도보다 작으면 그대로 사용

	// 최대 회전 각도보다 크면 최대 회전 각도로 보정
	fRad = fMaxRad / fRad;

	_vec3 vResult;
	D3DXVec3Lerp(&vResult, &v1, &v2, fRad);
	D3DXVec3Normalize(&vResult, &vResult);
	return vResult;
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
	for (_uint i = 0; i < 3; ++i)
	{
		Safe_Release(m_pNode[i]);
	}

	CGameObject::Free();
}
