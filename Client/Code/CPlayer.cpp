#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CCollisionMgr.h"
#include "CWarp.h"
#include "CSceneWarp.h"
#include "CTriggerPoint.h"
#include "CCollider.h"
#include "CCutSceneMgr.h"
#include "CSoundMgr.h"
#include "CEffectMgr.h"
#include "CItem.h"
#include "CMonster.h"
#include "CProjectile.h"
#include "CChargeArrow.h"
#include "CInteractionUI.h"
#include "CInteractMgr.h"
#include <IInteractable.h>

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pCalculatorCom(nullptr), m_pColliderCom(nullptr)
	, m_ePreState(PS_END), m_eCurState(PS_END), m_eWeaponType(WT_END), m_eWork(PW_NONE)
	, m_iMaxHp(0), m_fAttack(0.f), m_fSpeed(0.f), m_fPassion(0.f), m_fFaith(0.f)
	, m_strFrameKey(L"Default"), m_fFrame(0.f), m_fFrameEnd(0.f)
	, m_vDir(0.f,0.f,0.f), m_vPos(0.f, 0.f, 0.f), m_vLerpPos(0.f, 0.f, 0.f)
	, m_bRoll(false), m_bWorking(false), m_bIntro(false), m_bVillage(false), m_bCutScene(false)
	, m_iCombo(0), m_fCharge(0.f), m_fAccTime(0.f), m_fAccTime2(0.f)
	, m_pTriggerPoint(nullptr), m_pInteractionUI(nullptr), m_bCanTrigger(false), m_bMsgRegistered(false)
	, m_IsWarp(false), m_fWarpDelay(0.f), m_vWarpPos(0.f, 0.f, 0.f)
	, m_pChargeArrow(nullptr), m_fWorkSpeed(0.f), m_vWorkPos(0.f, 0.f, 0.f)
{
	D3DXMatrixIdentity(&m_matTex);
	ZeroMemory(m_vNormDir, sizeof(_vec3) * DIR_END);
}

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	:	CGameObject(pGraphicDev, StageChannel)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pCalculatorCom(nullptr), m_pColliderCom(nullptr)
	, m_ePreState(PS_END), m_eCurState(PS_END), m_eWeaponType(WT_END), m_eWork(PW_NONE)
	, m_iMaxHp(0), m_fAttack(0.f), m_fSpeed(0.f), m_fPassion(0.f), m_fFaith(0.f)
	, m_strFrameKey(L"Default"), m_fFrame(0.f), m_fFrameEnd(0.f)
	, m_vDir(0.f, 0.f, 0.f), m_vPos(0.f, 0.f, 0.f), m_vLerpPos(0.f, 0.f, 0.f)
	, m_bRoll(false), m_bWorking(false), m_bIntro(false), m_bVillage(false), m_bCutScene(false)
	, m_iCombo(0), m_fCharge(0.f), m_fAccTime(0.f), m_fAccTime2(0.f)
	, m_pTriggerPoint(nullptr), m_pInteractionUI(nullptr), m_bCanTrigger(false), m_bMsgRegistered(false)
	, m_IsWarp(false), m_fWarpDelay(0.f), m_vWarpPos(0.f, 0.f, 0.f)
	, m_pChargeArrow(nullptr), m_fWorkSpeed(0.f), m_vWorkPos(0.f, 0.f, 0.f)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	:	CGameObject(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pCalculatorCom(nullptr), m_pColliderCom(nullptr)
	, m_ePreState(PS_END), m_eCurState(PS_END), m_eWeaponType(WT_END), m_eWork(PW_NONE)
	, m_iMaxHp(0), m_fAttack(0.f), m_fSpeed(0.f), m_fPassion(0.f), m_fFaith(0.f)
	, m_strFrameKey(L"Default"), m_fFrame(0.f), m_fFrameEnd(0.f)
	, m_vDir(0.f, 0.f, 0.f), m_vPos(0.f, 0.f, 0.f), m_vLerpPos(0.f, 0.f, 0.f)
	, m_bRoll(false), m_bWorking(false), m_bIntro(false), m_bVillage(false), m_bCutScene(false)
	, m_iCombo(0), m_fCharge(0.f), m_fAccTime(0.f), m_fAccTime2(0.f)
	, m_pTriggerPoint(nullptr), m_pInteractionUI(nullptr), m_bCanTrigger(false), m_bMsgRegistered(false)
	, m_IsWarp(false), m_fWarpDelay(0.f), m_vWarpPos(0.f, 0.f, 0.f)
	, m_pChargeArrow(nullptr), m_fWorkSpeed(0.f), m_vWorkPos(0.f, 0.f, 0.f)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();

	Ready_Event();

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	if(!m_pMessageChannel) m_bMsgRegistered = false;
	else if (!m_bMsgRegistered) Ready_Event();

	//m_pTransformCom->Get_Info(INFO_POS, &m_vPrevPos);	(안쓰면 지우기)
	
	Key_Input(fTimeDelta);
	Move_Lerp(fTimeDelta);
	Charge(fTimeDelta);
	Move_Frame(fTimeDelta);
	Update_Warp(fTimeDelta);
	Execute_Work(fTimeDelta);
	Check_Scale();

	m_pInteractionUI->Update_GameObject(fTimeDelta);
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	Set_OnTerrain();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	m_pInteractionUI->LateUpdate_GameObject(fTimeDelta);
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	//------스프라이트 높이와 충돌체 위치 맞춤---------
	_float fY(m_vPos.y - m_pTransformCom->Get_Scale(ROT_Y) * 0.125f);
	AABB tAABB = { m_vPos.x, fY, m_vPos.z, 1.f, 1.f, 1.f };
	m_pColliderCom->Set_AABB(tAABB);
	m_pColliderCom->UpdateFromCustom(tAABB);
	//-------------------------------------------------

	// 충돌체 디버그용
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();

	m_pInteractionUI->UnActive();
	m_bCanTrigger = false;
	m_pTriggerPoint = nullptr;
}

void CPlayer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_TextureSet();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	/*_tchar szPos[256] = L"";
	swprintf_s(szPos, L"[플레이어] 플레이어 중점 위치 X : %f, Y : %f, Z : %f", m_vPos.x, m_vPos.y, m_vPos.z);
	OutputDebugString(szPos);
	OutputDebugString(L"\n");*/

	//_tchar szPos[256] = L"";
	//swprintf_s(szPos, L"a : %d, r : %d, g : %d, b : %d", Test_a, Test_r, Test_g, Test_b);
	//OutputDebugString(szPos);
	//OutputDebugString(L"\n");
}

void CPlayer::Ready_Variable()
{
	// Default
	m_eOBJID = OID_PLAYER;
	_float fScale = PLAYER_DEFAULT_SCALE;
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);		// Player 폴더
	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);

	// 상태
	m_eCurState = PS_IDLE;
	m_eWeaponType = WT_SWORD;

	// 능력치
	m_iHp = m_iMaxHp = PLAYER_DEFAULT_HP;
	m_fAttack = PLAYER_DEFAULT_ATTACK;
	m_fSpeed = PLAYER_DEFAULT_SPEED;
	m_fPassion = PLAYER_PASSION_MAX * 1.f;	// 100%
	m_fFaith = PLAYER_FAITH_MAX * 0.5f;		// 50%

	// 애니메이션 (상태머신이 돌면서 갱신될 것)

	// 이동
	_float fAngle(0.f);

	for (_uint i = 0; i < DIR_END; ++i)
	{
		m_vNormDir[i] = { cosf(fAngle), 0.f, -sinf(fAngle) };
		fAngle += D3DX_PI * 0.25f;
	}

	m_vDir = m_vNormDir[DIR_LEFT];

	// 상태 플래그 (이니셜라이저에서 모두 false)

	// 누적값 (이니셜라이저에서 모두 0 초기화)
	
	// 기타
	m_pInteractionUI = CInteractionUI::Create(m_pGraphicDev);
	m_fWorkSpeed = PLAYER_WORK_SPEED;

	// 컴포넌트
	m_pColliderCom->RegisterToManager(this, CL_PLAYER);
	AABB tAABB = { m_vPos.x, m_vPos.y, m_vPos.z, 1.f, 1.f, 1.f };
	m_pColliderCom->Set_AABB(tAABB);
}

void CPlayer::Ready_Event()
{
	if (m_pMessageChannel == nullptr) return;
	if (m_bMsgRegistered) return;

	//m_hmapSubHandles.insert({ L"StartGame.Move", m_pMessageChannel->Subscribe(L"Start_Game", [this](const IMessageChannel::EVENT& Event) {
	//	if (Event.eOBJID == this->Get_OBJID()) {
	//		m_pTransformCom->Set_Pos(10.f, 10.f, 10.f);
	//	}
	//	}) });

	m_hmapSubHandles.insert({ L"Player.Spawn", m_pMessageChannel->Subscribe(L"Player.Spawn", [this](const IMessageChannel::EVENT& Event) {
	_vec3 vSpawnPos = any_cast<_vec3>(Event.hmapData.find(L"Postion")->second);

	m_pTransformCom->Set_Pos(vSpawnPos.x, vSpawnPos.y, vSpawnPos.z);
	m_pTransformCom->Update_Component(0.f);
	}) });

	m_hmapSubHandles.insert({ L"Player_Damaged", m_pMessageChannel->Subscribe(L"Player.Attacked", [this](const IMessageChannel::EVENT& Event) {
	for (auto& Target : any_cast<vector<CGameObject*>>(Event.hmapData.find(L"Target")->second))
	{
		if (Target == this)
		{
			Attacked(any_cast<_float>(Event.hmapData.find(L"Attack")->second));
		}
	}
	}) });

	m_hmapSubHandles.insert({ L"CutScene.Dialogue", m_pMessageChannel->Subscribe(L"CutScene.Dialogue", [this](const IMessageChannel::EVENT& Event) {
	{

			if (!m_bCutScene && (m_eCurState != PS_REBIRTH)) {
				m_eCurState = PS_IDLE;
				m_iCombo = 0;
				m_bRoll = false;
				m_fCharge = 0.f;
				}

			auto CinemaTargetNameiter = Event.hmapData.find(L"CinemaTargetName");
			if (CinemaTargetNameiter == Event.hmapData.end()) { return; }
			auto Dothisiter = Event.hmapData.find(L"Dothis");
			if (Dothisiter == Event.hmapData.end()) { return; }
			if (any_cast<wstring>(CinemaTargetNameiter->second) == L"Player")
			{
				wstring strDothis = any_cast<wstring>(Dothisiter->second);
				if (strDothis == L"Start_Crying") {
					Set_Crying();
					m_pTransformCom->Set_Pos(-4.f, 0.f, 88.f);
					m_vPos = _vec3(-4.f, 2.f, 88.f);
					// 문제 생기면 정규화할것
					m_vDir = _vec3(1.f, 0.f, -1.f);
					return;
				}
				if (strDothis == L"Stop_Crying") {
					Set_StopCrying();
					return;
				}
				if (strDothis == L"Move_Gateway") {
					m_pTransformCom->Set_Pos(0.f, 0.f, 35.f);
					m_vPos = _vec3(0.f, 0.f, 35.f);
					return;
				}
				if (strDothis == L"LookforCam") {
					// 문제생기면 정규화 할것
					m_vDir = _vec3(1.f, 0.f, -1.f);
					return;
				}
			}
		}
	}) });

	m_hmapSubHandles.insert({ L"Tarot.Selected", m_pMessageChannel->Subscribe(L"Tarot.Selected", [this](const IMessageChannel::EVENT& Event)
		{
			auto TarotTypeiter = Event.hmapData.find(L"TarotType");
			if (TarotTypeiter == Event.hmapData.end()) { return; }
			_uint iTarotType = any_cast<_uint>(TarotTypeiter->second);

			switch (iTarotType - 1)
			{
			case 0:
				m_iMaxHp += 2;
				m_iHp += 2;
				break;
			case 2:
				m_fAttack = m_fAttack * 1.5f;
				break;
			}
		}
	) });

	m_hmapSubHandles.insert({ L"Player.AddPassion", m_pMessageChannel->Subscribe(L"Player.AddPassion", [this](const IMessageChannel::EVENT& Event)
		{
			m_fPassion += DEFAULT_PASSION_GAIN;

			if (m_fPassion > MAX_PASSION_VALUE)
				m_fPassion = MAX_PASSION_VALUE;
		}) });

	m_hmapSubHandles.insert({ L"Player.AddFaith", m_pMessageChannel->Subscribe(L"Player.AddFaith", [this](const IMessageChannel::EVENT& Event)
		{
			m_fFaith += DEFAULT_FAITH_GAIN;

			if (m_fPassion > MAX_FAITH_VALUE)
				m_fPassion = MAX_FAITH_VALUE;
		}) });

	m_hmapSubHandles.insert({ L"Trigger.Activate.Owner",m_pMessageChannel->Subscribe(L"Trigger.Activate.Owner" ,[this](const IMessageChannel::EVENT& Event)
{
		auto iter = Event.hmapData.find(L"Trigger_Name");
		if (iter == Event.hmapData.end())
			return;

		wstring name = any_cast<wstring>(iter->second);
		if (name == L"Sword")
		{
			// 검 먹은 로직
			m_eWeaponType = WT_SWORD;
			m_fAttack = 1.f;
		}
		else if (name == L"Gauntlet")
		{
			// 건틀릿 먹은 로직
			m_eWeaponType = WT_GAUNTLETS;
			m_fAttack = 2.f;
		}
}
) });

	m_bMsgRegistered = true;
}

HRESULT CPlayer::Add_Component()
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

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTextureSet*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlayerTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_TextureSet", pComponent });

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Calculator"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	Key_Input_Debug(fTimeDelta);

	Key_Input_Move(fTimeDelta);
		
	Key_Input_Interact(fTimeDelta);

	Key_Input_Combat(fTimeDelta);
}

void CPlayer::Key_Input_Debug(const _float& fTimeDelta)
{
	//for (int i = 0; i < 9; ++i)
	//{
	//	if (GetAsyncKeyState(i + 48))
	//	{	// 디버그용

	//		if (i == 8)
	//		{
	//			_vec3 vEffectPos{ m_vPos.x, 3.f, m_vPos.z };
	//			CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_ENEMYSPAWN, 0, vEffectPos);
	//		}
	//		else		CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_HIT, i, m_vPos);
	//	}
	//}

	//
	//if (CDInputMgr::GetInstance()->Key_Down(DIK_F1))
	//{	// 디버그용
	//	m_bIntro = !m_bIntro;
	//	if (m_bIntro)
	//	{
	//		m_fSpeed = PLAYER_INTRO_SPEED;
	//		_float fScale = PLAYER_INTRO_SCALE;
	//		m_pTransformCom->Set_Scale(fScale, fScale, fScale);
	//	}
	//	else
	//	{
	//		m_fSpeed = PLAYER_DEFAULT_SPEED;
	//		_float fScale = PLAYER_DEFAULT_SCALE;
	//		m_pTransformCom->Set_Scale(fScale, fScale, fScale);
	//	}
	//}
	//if (CDInputMgr::GetInstance()->Key_Down(DIK_F2))
	//{	// 디버그용
	//	if (!m_bIntro) return;
	//
	//	if (!m_bCutScene)
	//	{
	//		m_bCutScene = true;
	//		m_eCurState = PS_ACTION;
	//		m_strFrameKey = L"intro_kneel";
	//	}
	//	else
	//	{
	//		m_strFrameKey = L"intro_kneel-wake";
	//	}
	//}
	//if (CDInputMgr::GetInstance()->Key_Down(DIK_F3))
	//{	// 디버그용
	//	if (!m_bIntro) return;
	//
	//	m_eCurState = PS_REBIRTH;
	//	m_strFrameKey = L"intro_rebirth";
	//	_float fScale = PLAYER_REBIRTH_SCALE;
	//	m_pTransformCom->Set_Scale(fScale, fScale, fScale);
	//	m_pTransformCom->Set_Pos(m_vPos.x, 2.f, m_vPos.z);
	//}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_Z))
	{
		g_bDebug = !g_bDebug;
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_P))
	{
		Attacked(1.f);
	}
}

void CPlayer::Key_Input_Move(const _float& fTimeDelta)
{
	if (CCutSceneMgr::GetInstance()->Get_Playing()) { return; }
	if ((m_eCurState == PS_HIT) || (m_eCurState == PS_REBIRTH)) { return; }

	if (!m_bRoll && !m_iCombo && !m_fCharge && !m_bCutScene)
	{
		if (CDInputMgr::GetInstance()->Key_Pressing(DIK_W))
		{
			Stop_Work();

			m_eCurState = PS_RUN;

			if (CDInputMgr::GetInstance()->Key_Pressing(DIK_A)) m_vDir = m_vNormDir[DIR_LU];

			else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_D)) m_vDir = m_vNormDir[DIR_RU];

			else m_vDir = m_vNormDir[DIR_UP];

			m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
		}

		else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_S))
		{
			Stop_Work();

			m_eCurState = PS_RUN;

			if (CDInputMgr::GetInstance()->Key_Pressing(DIK_A)) m_vDir = m_vNormDir[DIR_LD];

			else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_D)) m_vDir = m_vNormDir[DIR_RD];

			else m_vDir = m_vNormDir[DIR_DOWN];

			m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
		}

		else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_A))
		{
			Stop_Work();

			m_eCurState = PS_RUN;
			m_vDir = m_vNormDir[DIR_LEFT];
			m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
		}

		else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_D))
		{
			Stop_Work();

			m_eCurState = PS_RUN;
			m_vDir = m_vNormDir[DIR_RIGHT];
			m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
		}

		else
		{
			if (m_eCurState != PS_REBIRTH && m_eCurState != PS_ACTION)
				m_eCurState = PS_IDLE;
		}
	}
}

void CPlayer::Key_Input_Interact(const _float& fTimeDelta)
{
	// 트리거 키인풋
	if (CDInputMgr::GetInstance()->Key_Down(DIK_E))
	{
		if (m_pTriggerPoint)
		{
			m_pTriggerPoint->Activate();
			return;
		}

		if (!m_bWorking)
		{
			_float fFindDist = 3.f;

			if (Find_WorkTarget(fFindDist))
				Start_Work();
		}
	}
}

void CPlayer::Key_Input_Combat(const _float& fTimeDelta)
{
	// 인트로 상태에서는 사용 불가
	if (m_bIntro || m_bCutScene) return;

	if (CDInputMgr::GetInstance()->Key_Down(DIK_SPACE))
	{
		if ((m_bRoll) || (m_iCombo) || (m_fCharge)) return;

		m_bRoll = true;
		m_eCurState = PS_ROLL;
		m_vLerpPos = m_vPos + m_vDir * 6.f;
	}

	if (m_bVillage) return;

	if (CDInputMgr::GetInstance()->Mouse_Down(DIM_LB))
	{
		_int iComboMax = Check_ComboMax();

		if ((m_iCombo == iComboMax) || (m_fCharge)) return;

		_float fComboDelay = Check_ComboDelay();

		if (m_fAccTime2 < fComboDelay) return;

		m_iCombo++;
		m_bRoll = false;
		m_fFrame = 0.f;
		m_eCurState = PS_ATTACK;
		m_vLerpPos = m_vPos + m_vDir * 1.f;
		m_fAccTime2 = 0.f;
		CSoundMgr::GetInstance()->Play(L"Player_Attack.wav", SOUND_EFFECT, 0.4f);
		Attack_HitBox();
	}

	if (CDInputMgr::GetInstance()->Mouse_Pressing(DIM_RB))	// 눌렀을 때 한 번만 true
	{
		if ((m_bRoll) || (m_iCombo)) return;

		if (!m_fCharge)
		{
			if (m_fPassion < 1.f) return;

			m_fPassion -= 1.f;

			m_eCurState = PS_CHARGE;
			m_strFrameKey = L"charge-start";
			m_fCharge += fTimeDelta;
		}

		// 화면 중앙 기준 방향 계산
		POINT pt{};
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		_float fCenterX = WINCX * 0.5f;
		_float fCenterZ = WINCY * 0.5f;

		_vec3 vDir;
		vDir.x = (pt.x - fCenterX);
		vDir.z = -(pt.y - fCenterZ);  // Y축 반전 (스크린→월드)
		vDir.y = 0.f;
		D3DXVec3Normalize(&m_vDir, &vDir);

		// 화살표 이펙트에 방향 전달
		if (!m_pChargeArrow)
		{
			m_pChargeArrow = static_cast<CChargeArrow*>(CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_INDICATOR_ARROW, 0, m_vPos, _vec3(0.f, 0.f, 1.f), this));
		}

		_vec3 vPos = m_vPos;
		_float fOffsetZ = 1.f;
		vPos.z -= fOffsetZ;
		m_pChargeArrow->Update_OwnerData(vPos, m_vDir, clamp((m_fCharge / PLAYER_CHARGE_MAX), 0.f, 1.f));
		m_pChargeArrow->Play();
	}
	else if (CDInputMgr::GetInstance()->Mouse_Up(DIM_RB))
	{
		if (m_strFrameKey == L"charge-start" || m_strFrameKey == L"charge-loop")
		{
			m_fFrame = 0.f;
			m_strFrameKey = L"charge-end";
			m_pChargeArrow->Stop();

			CProjectile* pTemp;
			CGameObject* pProjectile = pTemp = CProjectile::Create(m_pGraphicDev, m_vPos, m_vDir * 10.f, false, CL_PBULLET, D3DXCOLOR(1.f, 0.f, 0.4f, 1.f));

			_float fScale(1.5f + m_fCharge);
			static_cast<CTransform*>(pProjectile->Get_Component(ID_DYNAMIC, L"Com_Transform"))->Set_Scale(fScale, fScale, fScale);

			if (pProjectile)
			{
				wstring strObjTag = L"Projectile";

				IMessageChannel::EVENT EProjectile;
				EProjectile.strType = L"Obj.Add";
				EProjectile.eOBJID = Engine::OID_PROJECTILE;
				EProjectile.hmapData.emplace(L"Obj", pProjectile);
				EProjectile.hmapData.emplace(L"LayerTag", L"GameLogic_Layer");
				EProjectile.hmapData.emplace(L"ObjTag", strObjTag);
				m_pMessageChannel->Publish(EProjectile);
			}
		}
	}
}

void CPlayer::Set_OnTerrain()
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	Engine::CTerrainTex* pTerrainVtxCom = dynamic_cast<CTerrainTex*>(CManagement::GetInstance()->
		Get_Component(ID_STATIC, L"GameLogic_Layer", L"Terrain", L"Com_Buffer"));

	if (nullptr == pTerrainVtxCom)
		return;

	Engine::CTransform* pTerrainTransformCom = dynamic_cast<CTransform*>(CManagement::GetInstance()->
		Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Terrain", L"Com_Transform"));

	if (nullptr == pTerrainTransformCom)
		return;

	_matrix matInvTerrainWorld;
	_vec3   vLocalPos;

	D3DXMatrixInverse(&matInvTerrainWorld, 0, pTerrainTransformCom->Get_World());
	D3DXVec3TransformCoord(&vLocalPos, &vPos, &matInvTerrainWorld);

	_float		fHeight = m_pCalculatorCom->Compute_HeightOnTerrain(&vLocalPos, pTerrainVtxCom->Get_VtxPos(), VTXCNTX, VTXCNTZ);

	m_pTransformCom->Set_Pos(vPos.x, fHeight, vPos.z);
}

_vec3 CPlayer::Picking_OnTerrain()
{
	Engine::CTerrainTex* pTerrainVtxCom = dynamic_cast<CTerrainTex*>(CManagement::GetInstance()->
		Get_Component(ID_STATIC, L"GameLogic_Layer", L"Terrain", L"Com_Buffer"));

	if (nullptr == pTerrainVtxCom)
		return _vec3();

	Engine::CTransform* pTerrainTransformCom = dynamic_cast<CTransform*>(CManagement::GetInstance()->
		Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Terrain", L"Com_Transform"));

	if (nullptr == pTerrainTransformCom)
		return _vec3();

	return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, pTerrainVtxCom, pTerrainTransformCom);
}

void CPlayer::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case PS_IDLE:
	{
		if(m_bIntro)	m_fFrameEnd = _float(m_pTextureCom->Get_TextureEnd(L"intro_idle"));
		else			m_fFrameEnd = _float(m_pTextureCom->Get_TextureEnd(L"idle"));
	}
	break;

	case PS_RUN:
	{
		if (m_bIntro)	m_fFrameEnd = _float(m_pTextureCom->Get_TextureEnd(L"intro_run-up"));
		else			m_fFrameEnd = _float(m_pTextureCom->Get_TextureEnd(L"run-up"));
	}
	break;

	case PS_ROLL:
	{
		m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"roll-up");
	}
	break;

	case PS_ATTACK:
	{
		switch (m_eWeaponType)
		{
			case WT_SWORD:
				m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"attack-combo1");
				break;

			case WT_GAUNTLETS:
				m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"attack-gauntlets-combo1");
				break;
		}
	}
	break;

	case PS_CHARGE:
	{
		m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"charge-start");
	}
	break;

	case PS_HIT:
	{
		m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"knockback");
	}
	break;

	case PS_ACTION:
	{
		if (m_bIntro)	m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"intro_kneel");
		else
		{
			switch (m_eWork)
			{
			case PW_WOOD:
				m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"actions_chop-wood");
				break;

			case PW_ROCK:
				m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"actions_chop-stone");
				break;

			case PW_BUILD:
				m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"actions_hammer");
				break;
			}
		}
	}
	break;

	case PS_REBIRTH:
	{
		m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"intro_rebirth");
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CPlayer::Check_Scale()
{
	if ((m_eCurState == PS_ATTACK) && (m_iCombo == 4))
	{
		_float fScale = 11.f;
		m_pTransformCom->Set_Scale(fScale, fScale, fScale);
	}
	else if (m_eCurState != PS_REBIRTH)
	{
		_float fScale = PLAYER_DEFAULT_SCALE;
		m_pTransformCom->Set_Scale(fScale, fScale, fScale);
	}
}

void CPlayer::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += FRAME_SPEED * fTimeDelta;
	m_fAccTime += fTimeDelta;
	m_fAccTime2 += fTimeDelta;
	m_fFaith -= FAITH_DECREASE_RATE * fTimeDelta;

	if (m_fFaith < 0.f) m_fFaith = 0.f;

	if (m_fFrame > m_fFrameEnd)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
			// 구르기 및 공격 등은 프레임이 끝날 때까지 유지되었다가 종료
		case PS_ROLL:
		{
			m_bRoll = false;
		}
		break;

		case PS_ATTACK:
		{
			m_iCombo = 0;
		}
		break;

		case PS_CHARGE:
		{
			if (m_strFrameKey == L"charge-start") m_strFrameKey = L"charge-loop";
			else if (m_strFrameKey == L"charge-end")
			{
				m_fCharge = 0.f;
			}
		}
		break;

		case PS_HIT:
		{
			m_eCurState = PS_IDLE;
			m_iCombo = 0;
		}
		break;

		case PS_ACTION:
		{
			if		(m_strFrameKey == L"intro_kneel")		m_strFrameKey = L"intro_kneel-loop";
			else if (m_strFrameKey == L"intro_kneel-wake")
			{
				m_bCutScene = false;
				m_eCurState = PS_IDLE;
			}
		}
		break;

		case PS_REBIRTH:
		{
			m_bIntro = false;
			m_eCurState = PS_IDLE;
			m_fSpeed = PLAYER_DEFAULT_SPEED;
			_float fScale = PLAYER_DEFAULT_SCALE;
			m_pTransformCom->Set_Scale(fScale, fScale, fScale);
			m_pTransformCom->Set_Pos(m_vPos.x, 0.f, m_vPos.z);
		}
		break;
		}
	}
}

void CPlayer::Set_TextureSet()
{
	wstring strPreKey = m_strFrameKey;

	Set_FrameKey();

	if (m_strFrameKey != strPreKey) m_fFrame = 0.f;
	
	m_fFrameEnd = m_pTextureCom->Get_TextureEnd(m_strFrameKey); 

	_bool bFilpX = m_vDir.x > 0.f ? true : false;	// 반전 여부
	D3DXMatrixIdentity(&m_matTex);

	if (bFilpX)
	{
		m_matTex._11 *= -1.f;
		m_matTex._31 = 1.f;	// 반전 O : 오른쪽에서 왼쪽으로 읽음
	}

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(m_strFrameKey, _uint(m_fFrame));
}

void CPlayer::Set_FrameKey()
{
	if (m_bIntro)	Set_FrameKey_Intro();
	else			Set_FrameKey_Normal();
}

void CPlayer::Set_FrameKey_Intro()
{
	switch (m_eCurState)
	{
	case PS_IDLE:
	{
		if (m_vDir == m_vNormDir[DIR_UP] || m_vDir == m_vNormDir[DIR_LU] || m_vDir == m_vNormDir[DIR_RU]) m_strFrameKey = L"intro_idle-up";
		else m_strFrameKey = L"intro_idle";
	}
	break;

	case PS_RUN:
	{
		if (m_vDir == m_vNormDir[DIR_UP])										m_strFrameKey = L"intro_run-up";
		else if (m_vDir == m_vNormDir[DIR_DOWN])									m_strFrameKey = L"intro_run-down";
		else if (m_vDir == m_vNormDir[DIR_LD] || m_vDir == m_vNormDir[DIR_RD])	m_strFrameKey = L"intro_run-diagonal";
		else if (m_vDir == m_vNormDir[DIR_LEFT] || m_vDir == m_vNormDir[DIR_RIGHT])	m_strFrameKey = L"intro_run-horizontal";
		else if (m_vDir == m_vNormDir[DIR_LU] || m_vDir == m_vNormDir[DIR_RU])	m_strFrameKey = L"intro_run-up-diagonal";
	}
	break;

	case PS_CHARGE:
	case PS_ACTION:
	{
		// 키인풋 & Move_Frame에서 관리
	}
	break;
	}
}

void CPlayer::Set_FrameKey_Normal()
{
	switch (m_eCurState)
	{
	case PS_IDLE:
	{

		if (m_vDir == m_vNormDir[DIR_UP] || m_vDir == m_vNormDir[DIR_LU] || m_vDir == m_vNormDir[DIR_RU]) m_strFrameKey = L"idle-up";
		else m_strFrameKey = L"idle";
	}
	break;

	case PS_RUN:
	{
		if (m_vDir == m_vNormDir[DIR_UP])										m_strFrameKey = L"run-up";
		else if (m_vDir == m_vNormDir[DIR_DOWN])									m_strFrameKey = L"run-down";
		else if (m_vDir == m_vNormDir[DIR_LD] || m_vDir == m_vNormDir[DIR_RD])	m_strFrameKey = L"run-diagonal";
		else if (m_vDir == m_vNormDir[DIR_LEFT] || m_vDir == m_vNormDir[DIR_RIGHT])	m_strFrameKey = L"run-horizontal";
		else if (m_vDir == m_vNormDir[DIR_LU] || m_vDir == m_vNormDir[DIR_RU])	m_strFrameKey = L"run-up-diagonal";
	}
	break;

	case PS_ROLL:
	{
		if (m_vDir == m_vNormDir[DIR_UP] || m_vDir == m_vNormDir[DIR_LU] || m_vDir == m_vNormDir[DIR_RU])	m_strFrameKey = L"roll-up";
		else if (m_vDir == m_vNormDir[DIR_DOWN] || m_vDir == m_vNormDir[DIR_LD] || m_vDir == m_vNormDir[DIR_RD])	m_strFrameKey = L"roll-down";
		else if (m_vDir == m_vNormDir[DIR_LEFT] || m_vDir == m_vNormDir[DIR_RIGHT])									m_strFrameKey = L"roll-horizontal";
	}
	break;

	case PS_ATTACK:
	{
		switch (m_eWeaponType)
		{
		case WT_SWORD:
			if (m_iCombo == 1)	m_strFrameKey = L"attack-combo1";
			else if (m_iCombo == 2)	m_strFrameKey = L"attack-combo2";
			else if (m_iCombo == 3)	m_strFrameKey = L"attack-combo3";
			break;

		case WT_GAUNTLETS:
			if (m_iCombo == 1)	m_strFrameKey = L"attack-combo1-gauntlets";
			else if (m_iCombo == 2)	m_strFrameKey = L"attack-combo2-gauntlets";
			else if (m_iCombo == 3)	m_strFrameKey = L"attack-combo3-gauntlets";
			else if (m_iCombo == 4)	m_strFrameKey = L"attack-combo4-gauntlets";
			break;
		}
	}
	break;

	case PS_HIT:
		m_strFrameKey = L"knockback";
		break;

	case PS_CHARGE:
	case PS_ACTION:
	{
		// 키인풋 & Move_Frame에서 관리
	}
	break;
	}
}

_int CPlayer::Check_ComboMax()
{
	switch (m_eWeaponType)
	{
	case WT_SWORD:		return SWORD_COMBO_MAX;
	case WT_GAUNTLETS:	return GAUNTLETS_COMBO_MAX;
	default:			return 0;
	}
}

_float CPlayer::Check_ComboDelay()
{
	switch (m_eWeaponType)
	{
	case WT_SWORD:		return SWORD_COMBO_DELAY;
	case WT_GAUNTLETS:	return GAUNTLETS_COMBO_DELAY;
	default:			return 0;
	}
}

void CPlayer::Move_Lerp(const _float& fTimeDelta)
{
	if (!m_bRoll && !m_iCombo) return;
	
	D3DXVec3Lerp(&m_vPos, &m_vPos, &m_vLerpPos, LERF_FACTOR);

	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
}

void CPlayer::Charge(const _float& fTimeDelta)
{
	if (!m_fCharge) return;

	m_fCharge += PLAYER_CHARGE_SPEED * fTimeDelta;

	if(m_fCharge > PLAYER_CHARGE_MAX) m_fCharge = PLAYER_CHARGE_MAX;
}

void CPlayer::Set_Pos(const _vec3& vPos)
{
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_vPos = vPos;
	m_pTransformCom->Update_Component(0.f);
}


void CPlayer::Set_Tied()
{
	m_bIntro = true;
	m_fSpeed = PLAYER_INTRO_SPEED;
	_float fScale = 11.f;
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);
}

void CPlayer::Set_Crying()
{
	if (!m_bCutScene)
	{
		m_bCutScene = true;
		m_eCurState = PS_ACTION;
		m_strFrameKey = L"intro_kneel";
	}
}

void CPlayer::Set_StopCrying()
{
	if (m_bCutScene)
	{
		m_strFrameKey = L"intro_kneel-wake";
	}
}

void CPlayer::Set_Reborn()
{
	m_eCurState = PS_REBIRTH;
	m_strFrameKey = L"intro_rebirth";
	_float fScale = 20.f;
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);
	m_pTransformCom->Set_Pos(m_vPos.x, 2.f, m_vPos.z);
	m_vDir = _vec3(0.f, 0.f, -1.f);
	m_fSpeed = PLAYER_DEFAULT_SPEED;
}

void CPlayer::Set_MessageChannel(IMessageChannel* pMessageChannel)
{
	if (pMessageChannel == nullptr) { return; }
	if (m_pMessageChannel) {
		CGameObject::Unsubscribe_Handles();
		Safe_Release(m_pMessageChannel);
		m_bMsgRegistered = false;
	}
	m_pMessageChannel = pMessageChannel;
	m_pMessageChannel->AddRef();

	Ready_Event();
}

void CPlayer::Attack_HitBox()
{
	_float fX(0.f);

	if (m_vDir.x >= 0.f)	fX = m_vPos.x + 1.2f;
	else					fX = m_vPos.x - 1.2f;


	AABB tAABB = { fX, m_vPos.y, m_vPos.z - 1.f,
					2.f, 1.f, 2.f };

	if(g_bDebug) CRenderer::GetInstance()->Add_TestCollider(tAABB, 60);

	vector<CGameObject*> tempVec = CCollisionMgr::GetInstance()->Test_AABB(tAABB, CL_MONSTER);
	vector<CGameObject*> vecGrass = CCollisionMgr::GetInstance()->Test_AABB(tAABB, CL_GRASS);

	if (!tempVec.empty())
	{
		IMessageChannel::EVENT EAttack;
		EAttack.strType = L"Monster.Attacked";
		EAttack.eOBJID = Engine::OID_MONSTER;
		EAttack.hmapData.emplace(L"Attack", m_fAttack);
		EAttack.hmapData.emplace(L"Target", tempVec);
		m_pMessageChannel->Publish(EAttack);
		_tchar strSoundName[128] = L"";
		swprintf_s(strSoundName, L"Attacked%d.wav", Get_Rand_Int(1, 4));
		CSoundMgr::GetInstance()->Play(strSoundName, SOUND_EFFECT, 0.35f);

		_vec3 vEffectPos;
		for (auto& pObj : tempVec)
		{
			if (pObj->Get_OBJID() == OID_MONSTER)
			{
				static_cast<CMonster*>(pObj)->Get_EffectPos(&vEffectPos);
				CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_HIT, m_iCombo, vEffectPos, _vec3(0.2f, 0.2f, 0.f));
			}
		}
	}
	if (!vecGrass.empty())
	{
		IMessageChannel::EVENT EAttack;
		EAttack.strType = L"Grass.Attacked";
		EAttack.hmapData.emplace(L"Attack", m_fAttack);
		EAttack.hmapData.emplace(L"Target", vecGrass);
		m_pMessageChannel->Publish(EAttack);
		CSoundMgr::GetInstance()->Play(L"GrassHit.wav", SOUND_EFFECT, 0.35f);
	}
}

void CPlayer::Attacked(_float fDamage)
{
	if ((m_eCurState == PS_ROLL) || (m_fAccTime < PLAYER_INVINCIBLE_TIME)) return;

	if(m_iHp >= 2) m_iHp -= _int(fDamage);	// 시연용

	if (m_eCurState == PS_HIT) return;

	if (m_eCurState == PS_CHARGE)
	{
		if (m_strFrameKey == L"charge-end") return;
		else m_fCharge = 0.f;
	}
	else if (m_eCurState == PS_ACTION)
	{ m_bCutScene = false; return; }

	m_eCurState = PS_HIT;
	m_fAccTime = 0.f;
	CSoundMgr::GetInstance()->Play(L"Player_Hit.wav", SOUND_EFFECT, 0.4f);
	CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_PLAYERHIT, 6, _vec3(m_vPos.x, m_vPos.y - 1.f, m_vPos.z), _vec3(0.2f, 0.2f, 0.f));

	m_iCombo = 0;

	if(m_pChargeArrow) m_pChargeArrow->Stop();
}

void CPlayer::Update_Warp(const _float fTimeDelta)
{
	if (!m_IsWarp) { return; }

	if (m_fWarpDelay >= 0.5f)
	{
		m_pTransformCom->Set_Pos(m_vWarpPos.x, 0.f, m_vWarpPos.z);
		if (m_fWarpDelay >= 1.0f)
		{
			m_IsWarp = false;
			IMessageChannel::EVENT FadeEvent;
			FadeEvent.strType = L"Fade.Warp";
			FadeEvent.hmapData[L"Fade"] = wstring(L"FadeIn");
			m_pMessageChannel->Publish(FadeEvent);
			m_fWarpDelay = 0.f;
			return;
		}
	}
	m_fWarpDelay += fTimeDelta;

	
}

_bool CPlayer::Find_WorkTarget(const _float& fMaxDist)
{	// 작업 가능 여부를 반환 (가능한 경우, 타입 및 위치는 멤버 변수에 저장)
	_float fMinDist = fMaxDist;
	_bool  bFound = false;

	CInteractMgr* pInteractMgr = CInteractMgr::GetInstance();

	for (_uint i = CInteractMgr::WOOD; i <= CInteractMgr::BUILD; ++i)
	{
		CGameObject* pTarget = pInteractMgr->Find_Nearest(CInteractMgr::INTERACT_TYPE(i), m_vPos);

		if (!pTarget) continue;

		// 거리 계산
		IInteractable* pIObj = dynamic_cast<IInteractable*>(pTarget);
		_vec3 vWorkPos;
		pIObj->Get_WorkPos(&vWorkPos);
		vWorkPos = m_vPos - vWorkPos;
		vWorkPos.y = 0.f;
		_float fDist = D3DXVec3Length(&vWorkPos);

		if(fDist < fMinDist)
		{
			fMinDist = fDist;
			m_eWork = PLAYER_WORK(i);
			m_vWorkPos = vWorkPos;
			bFound = true;
		}
	}

	return bFound;
}

void CPlayer::Start_Work()
{
	m_bWorking = true;
	m_eCurState = PS_ACTION;

	switch (m_eWork)
	{
	case PW_WOOD:
		m_strFrameKey = L"actions_chop-wood";
		break;
	
	case PW_ROCK:
		m_strFrameKey = L"actions_chop-stone";
		break;
	
	case PW_BUILD:
		m_strFrameKey = L"actions_hammer";
		break;
	}
}

void CPlayer::Execute_Work(const _float& fTimeDelta)
{
	if(!m_bWorking) return;

	if(!CInteractMgr::GetInstance()->Apply_Work(CInteractMgr::INTERACT_TYPE(m_eWork), m_vPos, m_fWorkSpeed * fTimeDelta))
	{	// 작업 반영 실패(대상X, 거리 멂, 작업 완료) 시 작업 종료
		Stop_Work();
	}
}

void CPlayer::Stop_Work()
{
	if (!m_bWorking) return;

	m_bWorking = false;
	m_eWork = PW_NONE;
	m_eCurState = PS_IDLE;
}

void	CPlayer::OnCollision(CGameObject* pObject)
{
	if (pObject->Get_OBJID() == OID_MONSTER)
	{
		//m_pTransformCom->Set_Pos(10.f, 10.f, 10.f);
	}

	else if (pObject->Get_OBJID() == OID_ITEM)
	{
		if (static_cast<CItem*>(pObject)->Get_State() == CItem::IS_CHASE)
		{
			//CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_PICKUP, 0, _vec3(m_vPos.x, m_vPos.y - 1.f, m_vPos.z - 0.001f), _vec3(0.3f, 0.3f, 0.f));
			//CEffectMgr::GetInstance()->Create_Effect(CEffectMgr::EK_PICKUP, 1, _vec3(m_vPos.x, m_vPos.y - 1.f, m_vPos.z), _vec3(0.3f, 0.3f, 0.f));
		}
	}

	if (pObject->Get_OBJID() == OID_PROJECTILE)
	{
		if (!pObject->Get_Hp()) return;

		Attacked(1);
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
			else if(fOverlapX > fOverlapZ)
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
		}

		return;
	}
	if (pObject->Get_OBJID() == OID_TRIGGER)
	{
		if (m_bCutScene) { return; }
		m_bCanTrigger = true;
		m_pTriggerPoint = static_cast<CTriggerPoint*>(pObject);

		if (!m_pTriggerPoint->Get_Passive()) {
			m_pInteractionUI->Active();
		}
	}
	if (pObject->Get_OBJID() == OID_WARP)
	{
		if (m_eCurState == PS_ROLL) { return; }
		if (m_IsWarp)				{ return; }

		CWarp* Warp = static_cast<CWarp*>(pObject);

		if (Warp != nullptr)
		{
			_vec3 pos = Warp->Get_OtherWarp()->Get_WarpPos();
			m_vWarpPos = pos;
			m_IsWarp = true;
			IMessageChannel::EVENT FadeEvent;
			FadeEvent.strType = L"Fade.Warp";
			FadeEvent.hmapData[L"Fade"] = wstring(L"FadeOut");
			m_pMessageChannel->Publish(FadeEvent);
		}
	}
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev, StageChannel);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pPlayer;
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pPlayer;
}

void CPlayer::Free()
{
	if (m_pChargeArrow)
	{
		m_pChargeArrow->Set_Dead();  // CEffectMgr가 정리
		m_pChargeArrow = nullptr;
	}

	Safe_Release(m_pInteractionUI);
	CGameObject::Free();
}
