#include "pch.h"
#include "CMonsterB1.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CNode.h"
#include "CB1_AI.h"
#include "CProjectile.h"
#include <CMonsterN2.h>

CMonsterB1::CMonsterB1(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev),
	m_ePreState(B1S_END),
	m_eCurState(B1S_SPAWN),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_iPhase(0),
	m_iMaxHp(0)
{
	ZeroMemory(m_pNode, sizeof(m_pNode));
}

CMonsterB1::CMonsterB1(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CMonster(pGraphicDev, StageChannel),
	m_ePreState(B1S_END),
	m_eCurState(B1S_SPAWN),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_iPhase(0),
	m_iMaxHp(0)
{
	ZeroMemory(m_pNode, sizeof(m_pNode));
}


CMonsterB1::CMonsterB1(const CMonsterB1& rhs)
	: CMonster(rhs),
	m_ePreState(B1S_END),
	m_eCurState(B1S_SPAWN),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_iPhase(rhs.m_iPhase),
	m_iMaxHp(rhs.m_iPhase)
{
	memcpy(m_pNode, rhs.m_pNode, sizeof(m_pNode));
}

CMonsterB1::~CMonsterB1()
{
}

HRESULT CMonsterB1::Ready_GameObject()
{
	m_eOBJID = OID_MONSTER;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	return S_OK;
}

_int CMonsterB1::Update_GameObject(const _float& fTimeDelta)
{
	Check_Phase();

	Move_Frame(fTimeDelta);

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);
	
	//------스프라이트 높이와 충돌체 위치 맞춤---------
	_float fY(m_vPos.y - m_pTransformCom->Get_Scale(ROT_Y) * 0.125f);
	AABB tAABB = { m_vPos.x, fY, m_vPos.z, 2.5f, 2.5f, 2.5f };
	m_pColliderCom->Set_AABB(tAABB);
	//-------------------------------------------------

	// 충돌체 디버그용
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		return iExit;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	Compute_NodePos(fTimeDelta);

	return iExit;
}

void CMonsterB1::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Update_State();

	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);

	_vec3 vDir = *m_pAICom->Get_Dir();

	for (_uint i = 0; i < 4; ++i)
	{
		m_pNode[i]->LateUpdate_GameObject(fTimeDelta);

		if ((vDir.z > 0.f) && (	(m_eCurState != B1S_SHOOT)	&&
								(m_eCurState != B1S_SUMMON)	&&
								(m_eCurState != B1S_SPAWN)	&&
								(m_eCurState != B1S_ROAR)	))
			m_pNode[i]->Set_Depth(m_fDepth - (i + 1) * 0.001f);
		
		else
			m_pNode[i]->Set_Depth(m_fDepth + (i + 1) * 0.001f);
	}
}

void CMonsterB1::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();
	Set_Material();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	Reset_Material();
}

HRESULT CMonsterB1::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MonsterB1Texture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	// AI
	pComponent = m_pAICom = dynamic_cast<CB1_AI*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_B1_AI"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_DYNAMIC].insert({ L"Com_AI", pComponent });

	return S_OK;
}

void CMonsterB1::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = 10.f;
	m_fGroundY = -2.5f + fScale * 0.5f;
	m_iAttack = 1;
	m_iMaxHp = m_iHp = 10;
	m_iPhase = 1;

	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 10), m_fGroundY, _float(rand() % 10));
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_MONSTER);
	AABB tAABB = { m_vPos.x, m_vPos.y - 1.f, m_vPos.z, 2.5f, 2.5f, 2.5f };
	m_pColliderCom->Set_AABB(tAABB);

	// AI 세팅
	m_pAICom->Set_Owner(this);
	m_pAICom->Set_OwnerTransform(m_pTransformCom);
	m_pAICom->Set_TargetTransform(CPersistentMgr::GetInstance()->Get_PlayerTransform());
	m_pAICom->Set_State<MONSTER_B1_STATE>(B1S_SPAWN);
	m_pAICom->Set_GroundY(m_fGroundY);

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);

	// 마디 세팅
	_vec3 vScale{};
	_float fScaleReduction(0.8f);
	m_pTransformCom->Get_Scale(&vScale);
	vScale *= fScaleReduction;
	m_pNode[0] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_B1Node1Texture");
	m_pNode[0]->Set_NodeScale(vScale);
	m_pNode[0]->Set_UserID(CNode::MONSTER_B1);

	vScale *= fScaleReduction;
	m_pNode[1] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_B1Node2Texture");
	m_pNode[1]->Set_NodeScale(vScale);
	m_pNode[1]->Set_UserID(CNode::MONSTER_B1);

	vScale *= fScaleReduction;
	m_pNode[2] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_B1Node3Texture");
	m_pNode[2]->Set_NodeScale(vScale);
	m_pNode[2]->Set_UserID(CNode::MONSTER_B1);

	vScale *= fScaleReduction;
	m_pNode[3] = CNode::Create(m_pGraphicDev, m_pMessageChannel, m_pTransformCom, L"Proto_B1Node4Texture");
	m_pNode[3]->Set_NodeScale(vScale);
	m_pNode[3]->Set_UserID(CNode::MONSTER_B1);
}

void CMonsterB1::Ready_Event()
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

void CMonsterB1::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case B1S_CRAWL:
		m_fFrameEnd = 16.f;
		break;

	case B1S_JUMP:
		m_fFrameEnd = 16.f;
		break;

	case B1S_LAND:
		m_fFrameEnd = 19.f;
		break;

	case B1S_PREPARE:
	{
		m_fFrameEnd = 8.f;
		m_fAcmlTime = 0.f;

		for (_uint i = 0; i < 4; ++i)
		{
			m_pNode[i]->Set_AcmlTime(0);
			m_pNode[i]->Switch_UseMaterial();
		}
	}
		break;

	case B1S_ATTACK:
	{
		m_fFrameEnd = 19.f;
		
		for (_uint i = 0; i < 4; ++i)
		{
			m_pNode[i]->Switch_UseMaterial();
		}
	}
		break;

	case B1S_SHOOT:
		m_fFrameEnd = 36.f;
		break;

	case B1S_SUMMON:
		m_fFrameEnd = 19.f;
		break;

	case B1S_ROAR:
		m_fFrameEnd = 48.f;
		break;

	case B1S_SPAWN:
		m_fFrameEnd = 19.f;
		break;

	case B1S_STOP:
		m_fFrameEnd = 16.f;
		break;
	}

	m_ePreState = m_eCurState;
}

void CMonsterB1::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	m_fAcmlTime += fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case B1S_CRAWL:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B1S_STOP;
			break;

		case B1S_JUMP:
			m_fFrame = m_fFrameEnd - 0.001f;
			break;

		case B1S_LAND:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B1S_CRAWL;
			break;

		case B1S_ATTACK:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B1S_CRAWL;
			break;

		case B1S_SHOOT:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B1S_CRAWL;
			break;

		case B1S_SUMMON:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B1S_CRAWL;
			break;

		case B1S_SPAWN:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B1S_ROAR;
			break;

		case B1S_ROAR:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = B1S_CRAWL;
			break;
		}
	}
}

void CMonsterB1::Set_Texture()
{
	_vec3 vDir = *(m_pAICom->Get_Dir());		// AI로부터 받아온 방향
	_bool bFlipX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = _uint(m_fFrame);				// 현재 프레임
	_uint iTexIdx = _uint(m_eCurState);			// 텍스처 인덱스

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.25f;	// 세로는 4칸 고정(MonsterB1)

	switch (m_eCurState)
	{
	case B1S_CRAWL:
	case B1S_JUMP:
	case B1S_LAND:
	case B1S_PREPARE:
	case B1S_ATTACK:
		if (vDir.z > 0.f) iV += 2;
		break;

	case B1S_SHOOT:
	case B1S_SUMMON:
	case B1S_ROAR:
		break;

	case B1S_SPAWN:
		iTexIdx = 6;
		break;

	case B1S_STOP:
	{
		iTexIdx = 0;
		if (vDir.z > 0.f) iV += 2;
	}
	break;
	}

	if (bFlipX)
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

void CMonsterB1::Set_Material()
{
	if (m_eCurState != B1S_PREPARE) return;

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

void CMonsterB1::Reset_Material()
{
	if (!m_bMtrl) return;

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));

	m_bMtrl = false;
}

void CMonsterB1::Attack_HitBox()
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

void CMonsterB1::Attacked(const _int& iAttack)
{
	m_iHp -= iAttack;
}

void CMonsterB1::Update_State()
{
	if (m_eCurState == B1S_SPAWN || m_eCurState == B1S_ROAR) return;

	m_eCurState = m_pAICom->Get_RecommendState<MONSTER_B1_STATE>();
}

_vec3 CMonsterB1::Compute_LimitedDir(const _float& fMaxAngle, const _vec3& vCurDir, const _vec3& vDesiredDir)
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

void CMonsterB1::Compute_NodePos(const _float& fTimeDelta)
{	// Update에서 호출할 Node 위치 계산 함수
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

		if (m_eCurState != B1S_JUMP) vTargetPos.y = m_fGroundY * fScaleReduction;
		fScaleReduction *= fScaleReduction;

		vCurPos = m_pNode[i]->Get_NodePos();
		_vec3 vNewPos;
		D3DXVec3Lerp(&vNewPos, &vCurPos, &vTargetPos, fLerp);

		m_pNode[i]->Set_NodePos(vNewPos);
		m_pNode[i]->Set_NodeDir(vNewDir);
		m_pNode[i]->Update_GameObject(fTimeDelta);

		vPrevPos = vNewPos;
	}
}

void CMonsterB1::Check_Phase()
{
	_float fRatio = _float(m_iHp) / m_iMaxHp;

	switch (m_iPhase)
	{
	case 1:
		if (fRatio <= 0.5f)
		{
			m_iPhase = 2;
			m_pAICom->Push_Front_Pattern(B1S_SPAWN);
			m_pAICom->Set_Weight(B1S_PREPARE, 10);
		}
		return;

	case 2:
		return;
	}
}

void CMonsterB1::Launch_Projectile(const _uint& iCount)
{
	if (iCount > 1000) return;

	_float fRadian = 0.f;
	_float fGap = 2.f * D3DX_PI / iCount;
	_float fProjectileSpeed = 5.f;
	_vec3 vPos{ m_vPos.x, m_vPos.y - m_fGroundY, m_vPos.z };


	for (_uint i = 0; i < iCount; ++i)
	{
		_vec3 vSpeed{ cosf(fRadian) * fProjectileSpeed, 0.f, sinf(fRadian) * fProjectileSpeed };

		CGameObject* pProjectile = CProjectile::Create(m_pGraphicDev, vPos, vSpeed, false);

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

		fRadian += fGap;
	}
}

void CMonsterB1::Summon_Minion(const _uint& iCount)
{
	if (iCount > 1000) return;

	_float fRadian = 0.f;
	_float fGap = 2.f * D3DX_PI / iCount;
	_float fRadius = 10.f;

	for (_uint i = 0; i < iCount; ++i)
	{
		_vec3 vPos{ m_vPos.x + fRadius * cosf(fRadian), -1.f, m_vPos.z + fRadius * sinf(fRadian)};

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


CMonsterB1* CMonsterB1::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
	CMonsterB1* pMonster = new CMonsterB1(pGraphicDev, StageChannel);

	if (FAILED(pMonster->Ready_GameObject()))
	{
		Safe_Release(pMonster);
		MSG_BOX("pMonster Create Failed");
		return nullptr;
	}

	return pMonster;
}

void CMonsterB1::Free()
{
	for (_uint i = 0; i < 4; ++i)
	{
		Safe_Release(m_pNode[i]);
	}

	CGameObject::Free();
}
