#include "pch.h"
#include "CMonsterN3.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN3_AI.h"
#include "CMonsterHpBar.h"
#include "CSoundMgr.h"
#include "CEffectMgr.h"

CMonsterN3::CMonsterN3(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CMonster(pGraphicDev),
		m_ePreState(N3S_END),
		m_eCurState(N3S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CMonsterN3::CMonsterN3(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	:	CMonster(pGraphicDev, StageChannel),
		m_ePreState(N3S_END),
		m_eCurState(N3S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}


CMonsterN3::CMonsterN3(const CMonsterN3& rhs)
	:	CMonster(rhs),
		m_ePreState(N3S_END),
		m_eCurState(N3S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f)
{
}

CMonsterN3::~CMonsterN3()
{
}

HRESULT CMonsterN3::Ready_GameObject()
{
	m_eOBJID = OID_MONSTER;
	if (FAILED(Add_Component()))
		return E_FAIL;
	
	Ready_Variable();
	Ready_Event();

	return S_OK;
}

_int CMonsterN3::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	_vec3 vHpPos = m_vPos;
	vHpPos.y += 3.f;
	m_pHpBar->Set_TargetPos(vHpPos);
	m_pHpBar->Set_Hp(m_iHp);
	m_pHpBar->Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_PARTICLE_BLUE, 0, m_vEffectPos, _vec3(0.2f, 0.2f, 0.2f));
		return iExit;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CMonsterN3::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Update_State();

	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	//------스프라이트 높이와 충돌체 위치 맞춤---------
	m_pColliderCom->UpdateFromTransform(m_pTransformCom);
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();
	m_vEffectPos = { m_vPos.x, m_vPos.y + 0.5f, m_vPos.z };
	//-------------------------------------------------

	m_pHpBar->LateUpdate_GameObject(fTimeDelta);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMonsterN3::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	
	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CMonsterN3::OnCollision(CGameObject* pObject)
{
	if (pObject->Get_OBJID() == OID_PROJECTILE)
	{
		if (!pObject->Get_Hp()) return;

		_int iDamage = _int(static_cast<CTransform*>(pObject->Get_Component(ID_DYNAMIC, L"Com_Transform"))->Get_Scale(ROT_X));
		Attacked(iDamage);
	}

	if (pObject->Get_OBJID() == OID_BORDER)
	{
		_vec3 vCurPos;
		m_pTransformCom->Get_Info(INFO_POS, &vCurPos);

		Engine::CCollider* pBorderCol = dynamic_cast<Engine::CCollider*>(
			pObject->Get_Component(ID_STATIC, L"Com_Collider"));

		if (nullptr == pBorderCol)
			return;

		const Engine::AABB& borderAABB = pBorderCol->Get_AABB();

		const _float fHalf = 0.5f;

		_float fOverlapX = (borderAABB.hx + fHalf) - abs(vCurPos.x - borderAABB.x);
		_float fOverlapZ = (borderAABB.hz + fHalf) - abs(vCurPos.z - borderAABB.z);

		if (fOverlapX > 0.f && fOverlapZ > 0.f)
		{
			if (fOverlapX < fOverlapZ)
			{
				// X축 보정
				if (vCurPos.x < borderAABB.x)
					vCurPos.x = borderAABB.x - borderAABB.hx - fHalf - 0.01f;
				else
					vCurPos.x = borderAABB.x + borderAABB.hx + fHalf + 0.01f;
			}
			else if (fOverlapX > fOverlapZ)
			{
				// Z축 보정
				if (vCurPos.z < borderAABB.z)
					vCurPos.z = borderAABB.z - borderAABB.hz - fHalf - 0.01f;
				else
					vCurPos.z = borderAABB.z + borderAABB.hz + fHalf + 0.01f;
			}

			m_pTransformCom->Set_Pos(vCurPos.x, vCurPos.y, vCurPos.z);
			m_pTransformCom->Update_Component(0.f);
			m_pTransformCom->Compute_Bilboard(BBD_X);
			m_pAICom->Set_LerpPos(vCurPos);
		}

		return;
	}
}

HRESULT CMonsterN3::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MonsterN3Texture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	// AI
	pComponent = m_pAICom = dynamic_cast<CN3_AI*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_N3_AI"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_AI", pComponent });

	return S_OK;
}

void CMonsterN3::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = N3_DEFAULT_SCALE;
	m_fGroundY = -2.5f + fScale * 0.5f;
	m_iAttack = 1;
	m_iHp = N3_DEFAULT_HP;
	m_fHeight = N3_DEFAULT_HEIGHT;	// 공중에 떠있는 몬스터

	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 20), m_fGroundY + m_fHeight, _float(rand() % 20));
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_MONSTER);

	// AI 세팅
	m_pAICom->Set_OwnerTransform(m_pTransformCom);
	m_pAICom->Set_TargetTransform(CPersistentMgr::GetInstance()->Get_PlayerTransform());
	m_pAICom->Set_State<MONSTER_N3_STATE>(N3S_SPAWN);
	m_pAICom->Set_GroundY(m_fGroundY + m_fHeight);

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);

	m_pHpBar = CMonsterHpBar::Create(m_pGraphicDev, _float(m_iHp), m_vPos);
	m_pHpBar->UnActive();
}

void CMonsterN3::Ready_Event()
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

void CMonsterN3::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case N3S_FLY:
	{
		m_fFrameEnd = 16.f;
	}
	break;

	case N3S_PREPARE:
	{
		m_fFrameEnd = 28.f;
	}
	break;

	case N3S_RUSH:
	{
		m_fFrameEnd = 16.f;
	}
	break;

	case N3S_SPAWN:
	{
		m_fFrameEnd = 16.f;
	}
	break;

	case N3S_STOP:
	{
		m_fFrameEnd = 16.f;
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CMonsterN3::Move_Frame(const _float& fTimeDelta)
{
	_uint m_iPreAnim = _uint(m_fFrame);
	m_fFrame += m_fFrameSpeed * fTimeDelta;
	_uint m_iCurAnim = _uint(m_fFrame);

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case N3S_SPAWN:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = N3S_FLY;
			break;

		case N3S_PREPARE:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = N3S_RUSH;
			break;

		case N3S_RUSH:
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = N3S_FLY;
			break;
		}
	}
	else if (m_iPreAnim != m_iCurAnim)
	{
		if (m_eCurState == N3S_RUSH)
		{
			if ((m_iCurAnim == 1) ||
				(m_iCurAnim == 6) ||
				(m_iCurAnim == 11) ||
				(m_iCurAnim == 15)) Attack_HitBox();
		}
	}
}

void CMonsterN3::Set_Texture()
{
	_vec3 vDir = *(m_pAICom->Get_Dir());		// AI로부터 받아온 방향
	_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임
	_uint iTexIdx = _uint(m_eCurState);			// 텍스처 인덱스
	
	if (!m_pAICom->Is_Chasing() || m_eCurState == N3S_RUSH)
		bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	else
	{
		_vec3 vTargetDir = m_pAICom->Get_TargetDir();
		bFilpX = vTargetDir.x > 0.f ? true : false;	// 반전 여부
	}

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.5f;	// 세로는 2칸 고정(MonsterN3)

	switch (m_eCurState)
	{
	case N3S_FLY:
	{
	}
	break;

	case N3S_PREPARE:
	{
	}
	break;

	case N3S_RUSH:
	{
	}
	break;

	case N3S_SPAWN:
	{
		iTexIdx = 0;
	}
	break;

	case N3S_STOP:
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

	m_matTex._32 = _float(iV) * 0.5f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(iTexIdx);
}

void CMonsterN3::Attack_HitBox()
{
	AABB tAABB = { m_vPos.x, m_vPos.y, m_vPos.z,
					1.f, 1.f, 1.f };

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

void CMonsterN3::Attacked(const _int& iAttack)
{
	if (m_iHp > 0) 
	{
		m_iHp -= iAttack;
		m_pHpBar->Active();
	}

	_tchar strSoundName[128] = L"";
	swprintf_s(strSoundName, L"N1Hit%d.wav", Get_Rand_Int(1, 8));
	CSoundMgr::GetInstance()->Play(strSoundName, SOUND_HIT, 0.35f);
}

void CMonsterN3::Update_State()
{
	if (m_eCurState == N3S_SPAWN || m_eCurState == N3S_PREPARE || m_eCurState == N3S_RUSH)
		return;

	m_eCurState = m_pAICom->Get_RecommendState<MONSTER_N3_STATE>();
}

CMonsterN3* CMonsterN3::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
	CMonsterN3* pMonster = new CMonsterN3(pGraphicDev, StageChannel);

	if (FAILED(pMonster->Ready_GameObject()))
	{
		Safe_Release(pMonster);
		MSG_BOX("pMonster Create Failed");
		return nullptr;
	}

	return pMonster;
}

void CMonsterN3::Free()
{
	Safe_Release(m_pHpBar);
	CGameObject::Free();
}
