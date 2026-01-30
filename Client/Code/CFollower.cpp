#include "pch.h"
#include "CFollower.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CFollower_AI.h"
#include "CDInputMgr.h"
#include "CTriggerPoint.h"
#include "CSoundMgr.h"
#include "CEffectMgr.h"

CFollower::CFollower(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_ePreState(FOLLOWER_END),
	m_eCurState(FOLLOWER_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_ePreWork(FW_NONE),
	m_eCurWork(FW_NONE)
	, m_bUnConvert(false)
{
}

CFollower::CFollower(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_ePreState(FOLLOWER_END),
	m_eCurState(FOLLOWER_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_ePreWork(FW_NONE),
	m_eCurWork(FW_NONE)
	, m_bUnConvert(false)
{
}

CFollower::CFollower(const CFollower& rhs)
	: CGameObject(rhs),
	m_ePreState(FOLLOWER_END),
	m_eCurState(FOLLOWER_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_ePreWork(FW_NONE),
	m_eCurWork(FW_NONE)
	, m_bUnConvert(false)
{
}

CFollower::~CFollower()
{
}

HRESULT CFollower::Ready_GameObject()
{
	m_eOBJID = OID_NPC;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	return S_OK;
}

_int CFollower::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);
	Execute_Work(fTimeDelta);

	if ((m_eCurState != FOLLOWER_RECRUIT) && (m_eCurState != FOLLOWER_CONVERT)) {
		m_pTrigger->Update_GameObject(fTimeDelta);
	}

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		return iExit;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CFollower::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Update_State();
	Check_Frame();
	Check_BuildPriority();
	Check_Work();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);
		
	//------스프라이트 높이와 충돌체 위치 맞춤---------
	_float fY(m_vPos.y - m_pTransformCom->Get_Scale(ROT_Y) * 0.25f);
	AABB tAABB = { m_vPos.x, fY, m_vPos.z, 1.f, 1.f, 1.f };
	_vec3 vTriggerPos = { m_vPos.x, fY, m_vPos.z };
	m_pColliderCom->Set_AABB(tAABB);
	m_pColliderCom->UpdateFromAABB(tAABB);
	//-------------------------------------------------
	// 충돌체 디버그용

	if ((m_eCurState != FOLLOWER_RECRUIT) && (m_eCurState != FOLLOWER_CONVERT)) {
		m_pTrigger->Set_Pos_Trigger(vTriggerPos);
		m_pTrigger->LateUpdate_GameObject(fTimeDelta);
	}

	if (g_bDebug) m_pColliderCom->Update_AABBforRender();

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CFollower::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CFollower::OnCollision(CGameObject* pObject)
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

		const _float fHalf = 1.f;

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
			m_vLerpPos = vCurPos;

			_vec3 vDir = m_vPos - vCurPos;
			m_pAICom->Set_Dir(-*D3DXVec3Normalize(&vDir, &vDir));
			m_pAICom->Set_TargetTransform(nullptr);
		}

		return;
	}
}

void CFollower::WaitForCommand()
{
	m_eCurState = FOLLOWER_IDLE;
	m_eCurWork = FW_NONE;
	m_pAICom->Set_ActiveAI(false);
	m_pAICom->Set_State(FOLLOWER_IDLE);

	_tchar strSoundName[128] = L"";
	swprintf_s(strSoundName, L"follower talk%d.wav", Get_Rand_Int(1, 8));
	CSoundMgr::GetInstance()->Play(strSoundName, SOUND_DIALOGUE, 0.1f);
}

void CFollower::SetCommand(const FOLLOWER_WORK eWork)
{
	m_eCurWork = eWork;
	m_pAICom->Set_ActiveAI(true);
	m_pAICom->Set_TargetTransform(nullptr);
}

HRESULT CFollower::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(m_strProtoKey));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	// AI
	pComponent = m_pAICom = dynamic_cast<CFollower_AI*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Follower_AI"));

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_AI", pComponent });

	return S_OK;
}

void CFollower::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = 9.f;
	m_fGroundY = -2.5f + fScale * 0.5f - 1.5f;
	m_iHp = 10;
	m_eCurState = FOLLOWER_RECRUIT;
	m_eCurWork = FOLLOWER_WORK(Get_Rand_Int(1, 5));
	//m_eCurWork = FW_NONE;

	m_fWorkSpeed = FW_DEFAULT_WORK_SPEED + Get_Rand_Float(-0.002f, 0.002f);

	// Transform 세팅
	m_pTransformCom->Set_Pos(Get_Rand_Float(170.f, 230.f), m_fGroundY, Get_Rand_Float(10.f, 65.f));
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);
	m_pTransformCom->Update_Component(0.f);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);

	//m_pTransformCom->Get_Info(INFO_POS, &m_vPos);

	// Collider 세팅
	m_pColliderCom->RegisterToManager(this, CL_NPC);

	// AI 세팅
	m_pAICom->Set_OwnerTransform(m_pTransformCom);
	m_pAICom->Set_State<FOLLOWER_STATE>(FOLLOWER_RECRUIT);
	m_pAICom->Set_GroundY(m_fGroundY);

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);

	if (!m_bUnConvert)
	{
		m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, m_vPos, _vec3(1.f, 1.f, 1.f), Trigger::TI_FOLLOWER, L"Follower", false, this);
	}
	else
	{
		m_pTrigger = CTriggerPoint::Create(m_pGraphicDev, m_pMessageChannel, m_vPos, _vec3(1.f, 1.f, 1.f), Trigger::TI_FOLLOWER, L"UnConvert_Follower", false, this);

		m_hmapSubHandles.insert({ L"Trigger.Activate.Owner", m_pMessageChannel->Subscribe(L"Trigger.Activate.Owner", [this](const IMessageChannel::EVENT& Event)
			{
				CSoundMgr::GetInstance()->Play(L"follower warp in reverse effect.wav", SOUND_EFFECT, 0.35f);
				auto Nameiter = Event.hmapData.find(L"Trigger_Name");
				if (Nameiter == Event.hmapData.end()) { return; }
				auto Owneriter = Event.hmapData.find(L"Trigger_Owner");
				if (Owneriter == Event.hmapData.end()) { return; }

				if (any_cast<wstring>(Nameiter->second) == L"UnConvert_Follower")
				{
					if (any_cast<CGameObject*>(Owneriter->second) == this)
					{
						Safe_Destroy(m_pTrigger);
						m_eCurState = FOLLOWER_CONVERT;
						m_pAICom->Set_State<FOLLOWER_STATE>(FOLLOWER_CONVERT);
					}
				}
			}
		) });
	}
}

void CFollower::Ready_Event()
{
}

void CFollower::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case FOLLOWER_IDLE:
		m_fFrameEnd = 24.f;
		ReTarget();
		break;

	case FOLLOWER_RUN:
		m_fFrameEnd = 14.f;
		break;

	case FOLLOWER_DANCE:
		m_fFrameEnd = 47.f;
		break;

	case FOLLOWER_CHEER:
		m_fFrameEnd = 48.f;
		m_fFrame += Get_Rand_Int(0, 47);
		break;

	case FOLLOWER_TRANSFORM:
		m_fFrameEnd = 88.f;
		break;

	case FOLLOWER_UNCONVERT:
		m_fFrameEnd = 48.f;
		break;

	case FOLLOWER_CONVERT:
		m_fFrameEnd = 85.f;
		break;

	case FOLLOWER_ACTION:
	{
		switch (m_eCurWork)
		{
		case FW_NONE:	// (IDLE로 전환)
			m_eCurState = FOLLOWER_IDLE;
			m_fFrameEnd = 24.f;
			break;

		case FW_WOOD:
			m_fFrameEnd = 98.f;
			break;

		case FW_ROCK:
			m_fFrameEnd = 46.f;
			break;

		case FW_BUILD:
			m_fFrameEnd = 83.f;
			break;

		case FW_EAT:
			m_fFrameEnd = 40.f;
			break;

		case FW_PRAY:
			m_fFrameEnd = 60.f;
			break;
		}
	}
		break;

	case FOLLOWER_RECRUIT:
		m_iRecruitState = 0;
		m_fFrameEnd = 20.f;
		break;
	}

	m_ePreState = m_eCurState;
}

void CFollower::Move_Frame(const _float& fTimeDelta)
{
	_uint iPreAnimFrame = _uint(m_fFrame);
	m_fFrame += m_fFrameSpeed * fTimeDelta;
	_uint iCurAnimFrame = _uint(m_fFrame);

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		if (m_eCurState == FOLLOWER_RECRUIT)
		{
			if (m_iRecruitState == 0)
			{
				m_iRecruitState = 1;
				m_fFrameEnd = 96;
			}
			else if (m_iRecruitState == 1)
			{
				m_iRecruitState = 2;
				m_fFrameEnd = 75;
			}
			else if (m_iRecruitState == 2)
			{
				m_pAICom->Anim_End(m_eCurState);
				m_eCurState = FOLLOWER_IDLE;
			}
		}
		else if (m_eCurState == FOLLOWER_CONVERT)
		{
			if (m_bUnConvert)
			{
				m_iHp = 0;
				return;
			}
		}
		else if (m_eCurState == FOLLOWER_CHEER)
		{
			_uint iRand = Get_Rand_Int(0, 4);
			
			if (iRand == 0)
			{
				_vec3 vEffectPos = m_vPos;
				vEffectPos.y += 2.f;
				CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_PARTICLE_CONFETTI, 0, vEffectPos);
			}
		}
	}
	else if (iPreAnimFrame != iCurAnimFrame)
	{
		switch (m_eCurState)
		{
		case FOLLOWER_DANCE:
			break;
		case FOLLOWER_CHEER:
		{
			if (iCurAnimFrame == 1)
			{
				_int iRand = Get_Rand_Int(1, 3);

				if (iRand == 1)
				{
					_uint iChannel = Get_Rand_Int(SOUND_EFFECT1, SOUND_EFFECT10);
					_tchar strSoundName[128] = L"";
					swprintf_s(strSoundName, L"cheer%d.wav", Get_Rand_Int(1, 9));
					CSoundMgr::GetInstance()->Play(strSoundName, CHANNELID(iChannel), 0.02f);
				}
			}
		}
		break;

		case FOLLOWER_CONVERT:
		{
			if (iCurAnimFrame == 76) CSoundMgr::GetInstance()->Play(L"float follower.wav", SOUND_EFFECT, 0.35f);
		}
		break;
		}
	}
}

void CFollower::Set_Texture()
{
	_vec3 vDir = *(m_pAICom->Get_Dir());			// AI로부터 받아온 방향
	_bool bFilpX = vDir.x > 0.f ? true : false;		// 반전 여부
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임
	_uint iTexIdx = _uint(m_eCurState);
	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.125f;	// 세로는 8칸 고정(Follower)

	switch (m_eCurState)
	{
	case FOLLOWER_IDLE:
		break;

	case FOLLOWER_RUN:
		break;

	case FOLLOWER_DANCE:
		break;

	case FOLLOWER_CHEER:
		break;

	case FOLLOWER_TRANSFORM:
		break;

	case FOLLOWER_UNCONVERT:
		break;

	case FOLLOWER_CONVERT:
		break;

	case FOLLOWER_ACTION:
	{
		switch(m_eCurWork)
		{
		case FW_NONE:
			iTexIdx = 0;
			break;
		case FW_WOOD:
			iTexIdx = 7;
			break;
		case FW_ROCK:
			iTexIdx = 8;
			break;
		case FW_BUILD:
			iTexIdx = 9;
			break;
		case FW_EAT:
			iTexIdx = 10;
			break;
		case FW_PRAY:
			iTexIdx = 11;
			break;
		}
	}
		break;

	case FOLLOWER_RECRUIT:
	{
		switch (m_iRecruitState)
		{
		case 0:	// START
			iTexIdx = 12;
			break;
		case 1:	// LOOP
			iTexIdx = 13;
			break;
		case 2:	// END
			iTexIdx = 14;
			break;
		}
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

	m_matTex._32 = _float(iV) * 0.125f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(iTexIdx);
}

void CFollower::Update_State()
{
	if (m_eCurState == FOLLOWER_RECRUIT)
		return;

	m_eCurState = m_pAICom->Get_RecommendState<FOLLOWER_STATE>();
}

void CFollower::Check_Work()
{
	if (m_ePreWork == m_eCurWork)
		return;

	CGameObject* pTarget = nullptr;

	switch (m_eCurWork)
	{
	case FW_NONE:
		pTarget = nullptr;
		break;

	case FW_WOOD:
		pTarget = CInteractMgr::GetInstance()->Find_Nearest(CInteractMgr::WOOD, m_vPos);
		break;

	case FW_ROCK:
		pTarget = CInteractMgr::GetInstance()->Find_Nearest(CInteractMgr::ROCK, m_vPos);
		break;

	case FW_BUILD:
		pTarget = CInteractMgr::GetInstance()->Find_Nearest(CInteractMgr::BUILD, m_vPos);
		break;

	case FW_EAT:
		pTarget = CInteractMgr::GetInstance()->Find_Nearest(CInteractMgr::FOOD, m_vPos);
		break;

	case FW_PRAY:
		pTarget = CInteractMgr::GetInstance()->Find_Nearest(CInteractMgr::PRAY, m_vPos);
		break;
	}

	if (pTarget)
	{
		m_pAICom->Set_TargetTransform(static_cast<CTransform*>(pTarget->Get_Component(ID_DYNAMIC, L"Com_Transform")));
		m_ePreWork = m_eCurWork;
	}
	else
	{
		m_pAICom->Set_TargetTransform(nullptr);
		m_eCurWork = FOLLOWER_WORK(Get_Rand_Int(1, 5));
	}
}

void CFollower::Execute_Work(const _float& fTimeDelta)
{
	if (m_eCurState != FOLLOWER_ACTION) return;

	if (!m_bWorking)	// 작업 중이 아님 : 현재 위치 기준으로 대상 탐색 및 위치 저장
	{
		CGameObject* pTarget = CInteractMgr::GetInstance()->Find_Nearest(CInteractMgr::INTERACT_TYPE(m_eCurWork), m_vPos);

		if (pTarget)	// 타겟 존재 시 : 타겟 위치 저장 및 AI에 정보 전달, 작업 중으로 전환
		{
			CTransform* pTargetTC = static_cast<CTransform*>(pTarget->Get_Component(ID_DYNAMIC, L"Com_Transform"));
			m_pAICom->Set_TargetTransform(pTargetTC);
			pTargetTC->Get_Info(INFO_POS, &m_vWorkPos);
			m_bWorking = true;
		}
		else			// 타겟 존재 X : IDLE 상태로 전환
		{
			m_pAICom->Set_TargetTransform(nullptr);
			m_pAICom->Anim_End(m_eCurState);
			m_eCurState = FOLLOWER_IDLE;
			m_eCurWork = FOLLOWER_WORK(Get_Rand_Int(1, 5));
			return;
		}
		
	}

	// 작업 위치 기준으로 대상 탐색
	if (!CInteractMgr::GetInstance()->Apply_Work(	CInteractMgr::INTERACT_TYPE(m_eCurWork),
													m_vWorkPos,
													m_fWorkSpeed * fTimeDelta))
	{
		m_bWorking = false;	// 작업량 반영에 실패 시 작업이 끝난 것 : 작업에서 벗어남
 		m_pAICom->Anim_End(m_eCurState);
		m_pAICom->Set_TargetTransform(nullptr);
		m_eCurState = FOLLOWER_IDLE;

		// BUILD 완료 후 이전 작업으로 복귀
		if (FW_BUILD == m_eCurWork && m_eSavedWork != FW_NONE)
		{
			m_eCurWork = m_eSavedWork;
			m_eSavedWork = FW_NONE;
		}
		else
		{
			m_eCurWork = FOLLOWER_WORK(Get_Rand_Int(1, 5));
		}

		m_ePreWork = FW_NONE;
	}
}

void CFollower::Check_BuildPriority()
{
	if (FW_BUILD == m_eCurWork)
		return;

	if (CInteractMgr::GetInstance()->Has_BuildTarget())
	{
		m_eSavedWork = m_eCurWork;

		// BUILD로 전환
		m_eCurWork = FW_BUILD;
		m_ePreWork = FW_NONE;  // Check_Work()에서 타겟 재탐색
	}
}

CFollower* CFollower::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, const wstring& pProtoKey)
{
	CFollower* pFollower = new CFollower(pGraphicDev, StageChannel);

	pFollower->m_strProtoKey = pProtoKey;

	if (FAILED(pFollower->Ready_GameObject()))
	{
		Safe_Release(pFollower);
		MSG_BOX("pFollower Create Failed");
		return nullptr;
	}

	return pFollower;
}

CFollower* CFollower::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, const wstring& pProtoKey, const _vec3& vPos, FOLLOWER_STATE eState)
{
	CFollower* pFollower = new CFollower(pGraphicDev, StageChannel);

	pFollower->m_strProtoKey = pProtoKey;
	if (eState == FOLLOWER_UNCONVERT)
	{
		pFollower->m_bUnConvert = true;
	}

	if (FAILED(pFollower->Ready_GameObject()))
	{
		Safe_Release(pFollower);
		MSG_BOX("pFollower Create Failed");
		return nullptr;
	}

	pFollower->m_pTransformCom->Set_Pos(vPos.x, pFollower->m_fGroundY, vPos.z);
	pFollower->m_pTransformCom->Update_Component(0.f);
	pFollower->m_eCurState = eState;
	pFollower->m_pAICom->Set_State(eState);

	return pFollower;
}

void CFollower::Free()
{
	Safe_Destroy(m_pTrigger);
	CGameObject::Free();
}
