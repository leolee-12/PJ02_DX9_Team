#include "pch.h"
#include "CMonsterB2.h"
#include "CMonsterN1.h"
#include "CMonsterN2.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CNode.h"
#include "CB2_AI.h"
#include "CProjectile.h"

CMonsterB2::CMonsterB2(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev),
	m_ePreState(B2S_END),
	m_eCurState(B2S_SPAWN),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_fBtmPadding(0.f),
	m_iPhase(0),
	m_iMaxHp(0),
	m_fAcmlTime(0.f)
{
	ZeroMemory(m_pNode, sizeof(m_pNode));
}

CMonsterB2::CMonsterB2(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CMonster(pGraphicDev, StageChannel),
	m_ePreState(B2S_END),
	m_eCurState(B2S_SPAWN),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_fBtmPadding(0.f),
	m_iPhase(0),
	m_iMaxHp(0),
	m_fAcmlTime(0.f)
{
	ZeroMemory(m_pNode, sizeof(m_pNode));
}


CMonsterB2::CMonsterB2(const CMonsterB2& rhs)
	: CMonster(rhs),
	m_ePreState(B2S_END),
	m_eCurState(B2S_SPAWN),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_fBtmPadding(rhs.m_fBtmPadding),
	m_iPhase(rhs.m_iPhase),
	m_iMaxHp(rhs.m_iPhase),
	m_fAcmlTime(0.f)
{
	memcpy(m_pNode, rhs.m_pNode, sizeof(m_pNode));
}

CMonsterB2::~CMonsterB2()
{
}

HRESULT CMonsterB2::Ready_GameObject()
{
	m_eOBJID = OID_MONSTER;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	return S_OK;
}

_int CMonsterB2::Update_GameObject(const _float& fTimeDelta)
{
	Check_Phase();

	Move_Frame(fTimeDelta);

	Check_Status();

	for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
		pComponent.second->Update_Component(fTimeDelta);

	//if (iExit == DEAD)
	//{
	//	m_pColliderCom->UnregisterFromManager();
	//	return iExit;
	//}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	Compute_NodePos(fTimeDelta);

	return NOEVENT;
}

void CMonsterB2::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Update_State();

	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);

	_vec3 vDir = *m_pAICom->Get_Dir();

	//for (_uint i = 0; i < 4; ++i)
	//{
	//	m_pNode[i]->LateUpdate_GameObject(fTimeDelta);
	//
	//	if (vDir.z > 0.f)
	//		m_pNode[i]->Set_Depth(m_fDepth - (i + 1) * 0.001f);
	//
	//	else
	//		m_pNode[i]->Set_Depth(m_fDepth + (i + 1) * 0.001f);
	//}
}

void CMonsterB2::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_TextureSet();
	//Set_Material();

	//m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	//m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	//Reset_Material();
}

void CMonsterB2::OnCollision(CGameObject* pObject)
{
	CMonster::OnCollision(pObject);
}

HRESULT CMonsterB2::Add_Component()
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

	// TextureSet
	pComponent = m_pTexSetCom = dynamic_cast<Engine::CTextureSet*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MonsterB2Texture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	// AI
	pComponent = m_pAICom = dynamic_cast<CB2_AI*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_B2_AI"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_DYNAMIC].insert({ L"Com_AI", pComponent });

	return S_OK;
}

void CMonsterB2::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = 50.f;
	m_fBtmPadding = fScale * 0.51f;
	m_fGroundY = -2.5f + fScale * 0.5f - m_fBtmPadding;
	m_iAttack = 1;
	m_iMaxHp = m_iHp = 30;
	m_iPhase = 1;

	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 10), m_fGroundY, _float(rand() % 10));
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_MONSTER);
	AABB tAABB = { m_vPos.x, m_vPos.y, m_vPos.z, 5.f, 5.f, 5.f };
	m_pColliderCom->Set_AABB(tAABB);

	// AI 세팅
	m_pAICom->Set_Owner(this);
	m_pAICom->Set_OwnerTransform(m_pTransformCom);
	m_pAICom->Set_TargetTransform(CPersistentMgr::GetInstance()->Get_PlayerTransform());
	m_pAICom->Set_State<MONSTER_B2_STATE>(B2S_SPAWN);
	m_pAICom->Set_GroundY(m_fGroundY);

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);

	// 마디 세팅
	//_vec3 vScale{};
	//_float fScaleReduction(0.8f);
	//m_pTransformCom->Get_Scale(&vScale);
	//vScale *= fScaleReduction;
	//m_pNode[0] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_SpikeTexture");
	//m_pNode[0]->Set_NodeScale(vScale);
	//m_pNode[0]->Set_UserID(CNode::MONSTER_B2);

	//vScale *= fScaleReduction;
	//m_pNode[1] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_SpikeTexture");
	//m_pNode[1]->Set_NodeScale(vScale);
	//m_pNode[1]->Set_UserID(CNode::MONSTER_B2);

	//vScale *= fScaleReduction;
	//m_pNode[2] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_SpikeTexture");
	//m_pNode[2]->Set_NodeScale(vScale);
	//m_pNode[2]->Set_UserID(CNode::MONSTER_B2);

	//vScale *= fScaleReduction;
	//m_pNode[3] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_SpikeTexture");
	//m_pNode[3]->Set_NodeScale(vScale);
	//m_pNode[3]->Set_UserID(CNode::MONSTER_B2);
}

void CMonsterB2::Ready_Event()
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

void CMonsterB2::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case B2S_IDLE:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Idle");
		break;

	case B2S_DIG:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Dig");
		break;

	case B2S_ESCAPE:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Escape");
		break;

	case B2S_HIT:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Hit");
		break;

	case B2S_SMASH:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Smash");
		break;

	case B2S_SHOOT:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Summon");
		break;

	case B2S_SUMMON:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Summon");
		break;

	case B2S_SPAWN:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Spawn");
		break;

	case B2S_DIE:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Die");
		break;

	case B2S_DEAD:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Dead");
		break;

	case B2S_JUMP:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Jump");
		break;

	case B2S_DIVE:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Dive");
		break;

	case B2S_SPIKE1:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Spike");
		break;

	case B2S_SPIKE2:
		m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(L"BossLeshy_Spike");
		break;
	}

	m_ePreState = m_eCurState;
}

void CMonsterB2::Move_Frame(const _float& fTimeDelta)
{
	_uint iPreAnimFrame = _uint(m_fFrame);
	m_fFrame += m_fFrameSpeed * fTimeDelta;
	_uint iCurAnimFrame = _uint(m_fFrame);

	if(m_eCurState == B2S_IDLE) m_fFrame += m_fFrameSpeed * fTimeDelta;

	m_fAcmlTime += fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case B2S_IDLE:
			m_pAICom->Anim_End(m_eCurState);
			break;

		case B2S_DIG:
			m_fFrame = m_fFrameEnd;
			break;

		case B2S_ESCAPE:
		case B2S_HIT:
		case B2S_SPAWN:
		{
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B2S_IDLE;
		}
		break;

		case B2S_SMASH:
		case B2S_SHOOT:
		case B2S_SUMMON:
		case B2S_SPIKE1:
		case B2S_SPIKE2:
		{
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B2S_DIG;
		}
			break;

		case B2S_DIE:
		{
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B2S_DEAD;
		}
		break;

		case B2S_DEAD:
			m_fFrame = m_fFrameEnd;
			break;

		case B2S_JUMP:
			m_fFrame = m_fFrameEnd;
			break;

		case B2S_DIVE:
		{
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B2S_ESCAPE;
		}
		break;
		}
	}
	else if(iPreAnimFrame != iCurAnimFrame)
	{
		switch (m_eCurState)
		{
		case B2S_SMASH:
		{
			if		(iCurAnimFrame == 12) m_pAICom->Set_Signal(1);
			else if (iCurAnimFrame == 27) m_pAICom->Set_Signal(2);
			else if (iCurAnimFrame == 48) m_pAICom->Set_Signal(3);
		}
		break;

		case B2S_SHOOT:
		{
			if (iCurAnimFrame == 56 ||
				iCurAnimFrame == 62 ||
				iCurAnimFrame == 68) m_pAICom->Set_Signal();
		}
		break;

		case B2S_SUMMON:
		{
			if (iCurAnimFrame == 56) m_pAICom->Set_Signal();
		}
		break;
		}
	}
}

void CMonsterB2::Set_TextureSet()
{
	wstring strPreKey = m_strFrameKey;

	_uint iFrame = _uint(m_fFrame);				// 현재 프레임

	switch (m_eCurState)
	{
	case B2S_IDLE:
		m_strFrameKey = L"BossLeshy_Idle";
		break;

	case B2S_DIG:
		m_strFrameKey = L"BossLeshy_Dig";
		break;

	case B2S_ESCAPE:
		m_strFrameKey = L"BossLeshy_Escape";
		break;

	case B2S_SMASH:
		m_strFrameKey = L"BossLeshy_Smash";
		break;

	case B2S_SHOOT:
		m_strFrameKey = L"BossLeshy_Summon";
		break;

	case B2S_SUMMON:
		m_strFrameKey = L"BossLeshy_Summon";
		break;

	case B2S_SPAWN:
		m_strFrameKey = L"BossLeshy_Spawn";
		break;

	case B2S_DIE:
		m_strFrameKey = L"BossLeshy_Die";
		break;

	case B2S_DEAD:
		m_strFrameKey = L"BossLeshy_Dead";
		break;
	}

	if (m_strFrameKey != strPreKey) m_fFrame = 0.f;

	m_fFrameEnd = m_pTexSetCom->Get_TextureEnd(m_strFrameKey);

	//_vec3 vDir = *(m_pAICom->Get_Dir());		// AI로부터 받아온 방향
	//_bool bFlipX = vDir.x > 0.f ? true : false;	// 반전 여부
	//
	//if (bFlipX)
	//{
	//	m_matTex._11 *= -1.f;
	//	m_matTex._31 = 1.f;	// 반전 O : 오른쪽에서 왼쪽으로 읽음
	//}
	//
	//m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);
	//
	//D3DXMatrixIdentity(&m_matTex);

	m_pTexSetCom->Set_Texture(m_strFrameKey, _uint(m_fFrame));
}

void CMonsterB2::Set_Material()
{
	return;

	_float fMax = 1.f;
	_float fRatio = min(m_fAcmlTime / 2.f, 1.f);

	// 텍스처 색상 혼합
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	DWORD dwCol = DWORD(255 * fRatio * fMax);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, dwCol, DWORD(dwCol * 0.1f), DWORD(dwCol * 0.1f)));

	m_bMtrl = true;
}

void CMonsterB2::Reset_Material()
{
	if (!m_bMtrl) return;

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

	m_bMtrl = false;
}

void CMonsterB2::Attack_HitBox(_vec3 vPos)
{
	AABB tAABB = { vPos.x, vPos.y, vPos.z,
					3.f, 1.f, 3.f };

	vector<CGameObject*> tempVec = CCollisionMgr::GetInstance()->Test_AABB(tAABB, CL_PLAYER);

	if (g_bDebug) CRenderer::GetInstance()->Add_TestCollider(tAABB, 60);

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

void CMonsterB2::Attacked(const _int& iAttack)
{
	m_iHp -= iAttack;

	if (m_eCurState == B2S_IDLE)
	{
		m_pAICom->Set_State(B2S_HIT);
		m_eCurState = B2S_HIT;
		m_fFrame = 0.f;
	}
}

void CMonsterB2::Update_State()
{
	if (m_eCurState == B2S_SPAWN ||
		m_eCurState == B2S_ESCAPE ||
		m_eCurState == B2S_HIT) return;

	m_eCurState = m_pAICom->Get_RecommendState<MONSTER_B2_STATE>();
}

_vec3 CMonsterB2::Compute_LimitedDir(const _float& fMaxAngle, const _vec3& vCurDir, const _vec3& vDesiredDir)
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

void CMonsterB2::Compute_NodePos(const _float& fTimeDelta)
{	
	return;

	// Update에서 호출할 Node 위치 계산 함수
	_vec3 vCurPos;
	_vec3 vPrevPos = m_vPos;						// LateUpdate에서 갱신되지 않았으므로 이전 위치
	m_pTransformCom->Get_Info(INFO_POS, &vCurPos);	// AICom의 Update_Component에서 갱신된 현재 위치
	_vec3 vDir = *m_pAICom->Get_Dir();

	_vec3 vHeadVelocity = vCurPos - m_vPos;
	_float fHeadSpeed = D3DXVec3Length(&vHeadVelocity) / fTimeDelta;
	_float fBaseDist = 0.5f;
	_float fAdaptiveDist = fBaseDist + fHeadSpeed * 0.02f;
	_float fScaleReduction = 0.8f;

	for (_uint i = 0; i < 4; ++i)
	{
		_vec3 vDesiredDir = vPrevPos - m_pNode[i]->Get_NodePos();
		_vec3 vNewDir = Compute_LimitedDir(180.f * fTimeDelta, m_pNode[i]->Get_NodeDir(), vDesiredDir);

		_float fCurDist = D3DXVec3Length(&vDesiredDir);
		_float fDistRatio = fCurDist / fAdaptiveDist;
		_float fLerp = min(1.f, fDistRatio * 0.5f);
		_vec3 vTargetPos = vPrevPos - vNewDir * fAdaptiveDist;

		//if (m_eCurState != B2S_JUMP) vTargetPos.y = m_fGroundY * fScaleReduction;
		//fScaleReduction *= fScaleReduction;

		vCurPos = m_pNode[i]->Get_NodePos();
		_vec3 vNewPos;
		D3DXVec3Lerp(&vNewPos, &vCurPos, &vTargetPos, fLerp);

		m_pNode[i]->Set_NodePos(vNewPos);
		m_pNode[i]->Set_NodeDir(vNewDir);
		m_pNode[i]->Update_GameObject(fTimeDelta);

		vPrevPos = vNewPos;
	}
}

void CMonsterB2::Check_Phase()
{
	_float fRatio = _float(m_iHp) / m_iMaxHp;

	switch (m_iPhase)
	{
	case 1:
		if (fRatio <= 0.5f)
		{
			//m_iPhase = 2;
			//m_pAICom->Push_Front_Pattern(B2S_SUMMON);
			//m_pAICom->Set_Weight(B2S_PREPARE, 10);
		}
		return;

	case 2:
		return;
	}
}

void CMonsterB2::Check_Status()
{
	//m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	//------스프라이트 높이와 충돌체 위치 맞춤---------
	_float fY(0.f);
	if (m_eCurState == B2S_SPAWN || m_eCurState == B2S_DIG ||
		m_eCurState == B2S_JUMP || m_eCurState == B2S_DIVE)
		 fY = m_vPos.y - m_pTransformCom->Get_Scale(ROT_Y) * 0.24f;	// 충돌체 위치를 내려서 공격받지 않게 무적상태 구현
	else fY = m_vPos.y + m_pTransformCom->Get_Scale(ROT_Y) * 0.06f;
	AABB tAABB = { m_vPos.x, fY, m_vPos.z + 2.5f, 2.5f, 2.5f, 2.5f };
	m_pColliderCom->Set_AABB(tAABB);
	m_pColliderCom->UpdateFromCustom(tAABB);
	//-------------------------------------------------

	// 충돌체 디버그용
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();

	if ((m_iPhase != 0) && (m_iHp <= 0))
	{
		m_pAICom->Set_State(B2S_DIE);
		m_iPhase = 0;
	}
}

void CMonsterB2::Launch_Projectile(const _uint& iCount, const _vec3& vTargetDir)
{
	if (iCount > 1000) return;

	_float fBaseSpeed = 8.f;
	_float fBaseYSpeed = 10.f;
	_vec3 vPos{ m_vPos.x, m_vPos.y + 10.f, m_vPos.z };


	for (_uint i = 0; i < iCount; ++i)
	{
		_float fRandX = Get_Rand_Float(-5.f, 5.f);
		_float fRandY = Get_Rand_Float(-5.f, 5.f);
		_float fRandZ = Get_Rand_Float(-5.f, 5.f);

		_vec3 vSpeed{	vTargetDir.x * fBaseSpeed + fRandX,
						fBaseYSpeed + fRandY,
						vTargetDir.z * fBaseSpeed + fRandZ };

		CGameObject* pProjectile = CProjectile::Create(m_pGraphicDev, vPos, vSpeed, true);

		if (pProjectile)
		{
			IMessageChannel::EVENT EProjectile;
			EProjectile.strType = L"Obj.Add";
			EProjectile.eOBJID = Engine::OID_PROJECTILE;
			EProjectile.hmapData.emplace(L"Obj", pProjectile);
			EProjectile.hmapData.emplace(L"LayerTag", L"GameLogic_Layer");
			EProjectile.hmapData.emplace(L"ObjTag", L"Projectile");
			m_pMessageChannel->Publish(EProjectile);
		}
	}
}

void CMonsterB2::Summon_Minion(const _uint& iCount)
{
	if (iCount > 1000) return;

	_float fRadian = 0.f;
	_float fGap = 2.f * D3DX_PI / iCount;
	_float fRadius = 10.f;

	for (_uint i = 0; i < iCount; ++i)
	{
		_vec3 vPos{ m_vPos.x + fRadius * cosf(fRadian), -1.f, m_vPos.z + fRadius * sinf(fRadian) };

		CGameObject* pMonster = CMonsterN2::Create(m_pGraphicDev, m_pMessageChannel, vPos);

		if (pMonster)
		{
			IMessageChannel::EVENT ESummonMonster;
			ESummonMonster.strType = L"Obj.Add";
			ESummonMonster.eOBJID = Engine::OID_MONSTER;
			ESummonMonster.hmapData.emplace(L"Obj", pMonster);
			ESummonMonster.hmapData.emplace(L"LayerTag", L"GameLogic_Layer");
			ESummonMonster.hmapData.emplace(L"ObjTag", L"Monster");
			m_pMessageChannel->Publish(ESummonMonster);
		}

		fRadian += fGap;
	}
}

CMonsterB2* CMonsterB2::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
	CMonsterB2* pMonster = new CMonsterB2(pGraphicDev, StageChannel);

	if (FAILED(pMonster->Ready_GameObject()))
	{
		Safe_Release(pMonster);
		MSG_BOX("pMonster Create Failed");
		return nullptr;
	}

	return pMonster;
}

void CMonsterB2::Free()
{
	for (_uint i = 0; i < 4; ++i)
	{
		Safe_Release(m_pNode[i]);
	}

	CGameObject::Free();
}
