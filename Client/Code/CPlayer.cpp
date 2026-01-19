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
#include "Engine_Struct.h"
#include "CCollider.h"
#include "CCutSceneMgr.h"
#include "CEffect.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_ePreState(PS_END),
	m_eCurState(PS_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f),
	m_fSpeed(0.f),
	m_iAttack(0),
	m_bRoll(false),
	m_iCombo(0),
	m_fLerp(0.2f),
	m_fCharge(0.f),
	m_fChargeMax(3.f),
	m_bMsgRegistered(false),
	m_bIntro(false),
	m_bAction(false)
{
}

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	:	CGameObject(pGraphicDev, StageChannel),
		m_ePreState(PS_END),
		m_eCurState(PS_IDLE),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_fSpeed(0.f),
		m_iAttack(0),
		m_bRoll(false),
		m_iCombo(0),
		m_fLerp(0.2f),
		m_fCharge(0.f),
		m_fChargeMax(3.f),
		m_bMsgRegistered(false),
		m_bIntro(false),
		m_bAction(false)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	:	CGameObject(rhs),
		m_ePreState(PS_END),
		m_eCurState(PS_IDLE),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_fSpeed(rhs.m_fSpeed),
		m_iAttack(rhs.m_iAttack),
		m_bRoll(false),
		m_iCombo(0),
		m_vPos(rhs.m_vPos),
		m_fLerp(rhs.m_fLerp),
		m_fCharge(0.f),
		m_fChargeMax(rhs.m_fChargeMax),
		m_bMsgRegistered(rhs.m_bMsgRegistered),
		m_bIntro(rhs.m_bIntro),
		m_bAction(false)
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

	m_pTransformCom->Get_Info(INFO_POS, &m_vPrevPos);
	
	Key_Input(fTimeDelta);
	Move_Lerp(fTimeDelta);
	Charge(fTimeDelta);
	Move_Frame(fTimeDelta);
	Update_Warp(fTimeDelta);
	
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

	CGameObject::LateUpdate_GameObject(fTimeDelta);

	//------스프라이트 높이와 충돌체 위치 맞춤---------
	_float fY(m_vPos.y - m_pTransformCom->Get_Scale(ROT_Y) * 0.125f);
	AABB tAABB = { m_vPos.x, fY, m_vPos.z, 1.f, 1.f, 1.f };
	m_pColliderCom->Set_AABB(tAABB);
	m_pColliderCom->UpdateFromCustom(tAABB);
	//-------------------------------------------------

	// 충돌체 디버그용
	if (g_bDebug) m_pColliderCom->Update_AABBforRender();

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

	_tchar szPos[256] = L"";
	swprintf_s(szPos, L"[플레이어] 플레이어 중점 위치 X : %f, Y : %f, Z : %f", m_vPos.x, m_vPos.y, m_vPos.z);
	OutputDebugString(szPos);
	OutputDebugString(L"\n");
}

void CPlayer::Ready_Variable()
{
	m_bIntro = false;
	m_fSpeed = PLAYER_DEFAULT_SPEED;
	m_iAttack = PLAYER_DEFAULT_ATTACK;
	m_iHp = 8;
	m_fAcmlTime = 0.f;

	m_eOBJID = OID_PLAYER;
	_float fScale = PLAYER_DEFAULT_SCALE;
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);		// Player 폴더
	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);
	m_fFrameSpeed = 24.f;

	m_pColliderCom->RegisterToManager(this, CL_PLAYER);
	AABB tAABB = { m_vPos.x, m_vPos.y, m_vPos.z, 1.f, 1.f, 1.f };
	m_pColliderCom->Set_AABB(tAABB);

	_float fAngle(0.f);

	for (_uint i = 0; i < DIR_END; ++i)
	{
		m_vNormDir[i] = { cosf(fAngle), 0.f, -sinf(fAngle) };
		fAngle += D3DX_PI * 0.25f;
	}

	m_vDir = m_vNormDir[DIR_LEFT];
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
			Attacked(any_cast<_int>(Event.hmapData.find(L"Attack")->second));
		}

	}
	}) });

	m_hmapSubHandles.insert({ L"CutScene.Dialogue", m_pMessageChannel->Subscribe(L"CutScene.Dialogue", [this](const IMessageChannel::EVENT& Event) {
	{

			if (!m_bAction && (m_eCurState != PS_REBIRTH)) {
				m_eCurState = PS_IDLE;
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
	for (int i = 0; i < 8; ++i)
	{
		if (GetAsyncKeyState(i + 48))
		{	// 디버그용

			_vec3 vPos{ m_vPos.x + 0.2f, m_vPos.y - 1.f, m_vPos.z };
			CGameObject* pEffect = CEffect::Create(m_pGraphicDev, vPos, i);

			if (pEffect)
			{
				wstring strObjTag = L"Effect";

				IMessageChannel::EVENT ESummonMonster;
				ESummonMonster.strType = L"Obj.Add";
				ESummonMonster.eOBJID = Engine::OID_EFFECT;
				ESummonMonster.hmapData.emplace(L"Obj", pEffect);
				ESummonMonster.hmapData.emplace(L"LayerTag", L"GameLogic_Layer");
				ESummonMonster.hmapData.emplace(L"ObjTag", strObjTag);
				m_pMessageChannel->Publish(ESummonMonster);
			}
		}
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_Z))
	{
		g_bDebug = !g_bDebug;
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_F1))
	{	// 디버그용
		m_bIntro = !m_bIntro;
		if (m_bIntro)
		{
			m_fSpeed = PLAYER_INTRO_SPEED;
			_float fScale = PLAYER_INTRO_SCALE;
			m_pTransformCom->Set_Scale(fScale, fScale, fScale);
		}
		else
		{
			m_fSpeed = PLAYER_DEFAULT_SPEED;
			_float fScale = PLAYER_DEFAULT_SCALE;
			m_pTransformCom->Set_Scale(fScale, fScale, fScale);
		}
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_F2))
	{	// 디버그용
		if (!m_bIntro) return;

		if (!m_bAction)
		{
			m_bAction = true;
			m_eCurState = PS_ACTION;
			m_strFrameKey = L"intro_kneel";
		}
		else
		{
			m_strFrameKey = L"intro_kneel-wake";
		}
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_F3))
	{	// 디버그용
		if (!m_bIntro) return;

		m_eCurState = PS_REBIRTH;
		m_strFrameKey = L"intro_rebirth";
		_float fScale = PLAYER_REBIRTH_SCALE;
		m_pTransformCom->Set_Scale(fScale, fScale, fScale);
		m_pTransformCom->Set_Pos(m_vPos.x, 2.f, m_vPos.z);
	}

	if (CCutSceneMgr::GetInstance()->Get_Playing())				{ return; }
	if ((m_eCurState == PS_HIT) || (m_eCurState == PS_REBIRTH))	{ return; }

	if (!m_bRoll && !m_iCombo && !m_fCharge && !m_bAction)
	{
		if (CDInputMgr::GetInstance()->Key_Pressing(DIK_W))
		{
			m_eCurState = PS_RUN;

			if (CDInputMgr::GetInstance()->Key_Pressing(DIK_A)) m_vDir = m_vNormDir[DIR_LU];

			else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_D)) m_vDir = m_vNormDir[DIR_RU];

			else m_vDir = m_vNormDir[DIR_UP];

			m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
		}

		else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_S))
		{
			m_eCurState = PS_RUN;

			if (CDInputMgr::GetInstance()->Key_Pressing(DIK_A)) m_vDir = m_vNormDir[DIR_LD];

			else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_D)) m_vDir = m_vNormDir[DIR_RD];

			else m_vDir = m_vNormDir[DIR_DOWN];

			m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
		}

		else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_A))
		{
			m_eCurState = PS_RUN;
			m_vDir = m_vNormDir[DIR_LEFT];
			m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
		}

		else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_D))
		{
			m_eCurState = PS_RUN;
			m_vDir = m_vNormDir[DIR_RIGHT];
			m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
		}

		else
		{
			if (m_eCurState != PS_REBIRTH)
			m_eCurState = PS_IDLE;
		}
	}

	// 트리거 키인풋
	if (CDInputMgr::GetInstance()->Key_Down(DIK_E))
	{
		if (!m_pTriggerPoint) { return; }

		m_pTriggerPoint->Activate();
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_P))
	{
		Attacked(1);
		//m_iHp -= 1;
	}

	// 인트로 상태에서는 사용 불가
	if (m_bIntro) return;

	if (CDInputMgr::GetInstance()->Key_Down(DIK_SPACE))
	{
		if ((m_bRoll) || (m_iCombo) || (m_fCharge)) return;

		m_bRoll = true;
		m_eCurState = PS_ROLL;
		m_vLerpPos = m_vPos + m_vDir * 6.f;
		m_fLerp = 0.2f;
	}

	if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)	// 눌렀을 때 한 번만 true
	{			
		if ((m_iCombo == 3) || (m_fCharge)) return;

		m_iCombo++;
		m_bRoll = false;
		m_fFrame = 0.f;
		m_eCurState = PS_ATTACK;
		//m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed * 3.f);
		m_vLerpPos = m_vPos + m_vDir * 1.f;
		m_fLerp = 0.2f;
		Attack_HitBox();
	}

	if (GetAsyncKeyState(VK_RBUTTON) & 0x0001)	// 눌렀을 때 한 번만 true
	{
		if ((m_bRoll) || (m_iCombo)) return;
	
		if (!m_fCharge)
		{
			m_eCurState = PS_CHARGE;
			m_strFrameKey = L"charge-start";
			m_fCharge += fTimeDelta;
		}
		else if(m_strFrameKey == L"charge-loop")
		{
			m_fFrame = 0.f;
			m_fChargeMax = m_fCharge;
			m_strFrameKey = L"charge-end";
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
		m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"attack-combo1");
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
		else			m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"action");
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

void CPlayer::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;
	m_fAcmlTime += fTimeDelta;

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
				m_fChargeMax = 3.f;
			}
		}
		break;

		case PS_HIT:
		{
			m_eCurState = PS_IDLE;
		}
		break;

		case PS_ACTION:
		{
			if		(m_strFrameKey == L"intro_kneel")		m_strFrameKey = L"intro_kneel-loop";
			else if (m_strFrameKey == L"intro_kneel-wake")
			{
				m_bAction = false;
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
	if (m_bIntro)
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
			if		(m_vDir == m_vNormDir[DIR_UP])										m_strFrameKey = L"intro_run-up";
			else if (m_vDir == m_vNormDir[DIR_DOWN])									m_strFrameKey = L"intro_run-down";
			else if (m_vDir == m_vNormDir[DIR_LD]	|| m_vDir == m_vNormDir[DIR_RD])	m_strFrameKey = L"intro_run-diagonal";
			else if (m_vDir == m_vNormDir[DIR_LEFT] || m_vDir == m_vNormDir[DIR_RIGHT])	m_strFrameKey = L"intro_run-horizontal";
			else if (m_vDir == m_vNormDir[DIR_LU]	|| m_vDir == m_vNormDir[DIR_RU])	m_strFrameKey = L"intro_run-up-diagonal";
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
	else
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
			if		(m_vDir == m_vNormDir[DIR_UP])										m_strFrameKey = L"run-up";
			else if (m_vDir == m_vNormDir[DIR_DOWN])									m_strFrameKey = L"run-down";
			else if (m_vDir == m_vNormDir[DIR_LD]	|| m_vDir == m_vNormDir[DIR_RD])	m_strFrameKey = L"run-diagonal";
			else if (m_vDir == m_vNormDir[DIR_LEFT] || m_vDir == m_vNormDir[DIR_RIGHT])	m_strFrameKey = L"run-horizontal";
			else if (m_vDir == m_vNormDir[DIR_LU]	|| m_vDir == m_vNormDir[DIR_RU])	m_strFrameKey = L"run-up-diagonal";
		}
		break;

		case PS_ROLL:
		{
			if		(m_vDir == m_vNormDir[DIR_UP]	|| m_vDir == m_vNormDir[DIR_LU] || m_vDir == m_vNormDir[DIR_RU])	m_strFrameKey = L"roll-up";
			else if (m_vDir == m_vNormDir[DIR_DOWN] || m_vDir == m_vNormDir[DIR_LD] || m_vDir == m_vNormDir[DIR_RD])	m_strFrameKey = L"roll-down";
			else if (m_vDir == m_vNormDir[DIR_LEFT] || m_vDir == m_vNormDir[DIR_RIGHT])									m_strFrameKey = L"roll-horizontal";
		}
		break;

		case PS_ATTACK:
		{
			if		(m_iCombo == 1)	m_strFrameKey = L"attack-combo1";
			else if (m_iCombo == 2)	m_strFrameKey = L"attack-combo2";
			else if (m_iCombo == 3)	m_strFrameKey = L"attack-combo3";
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
}

void CPlayer::Move_Lerp(const _float& fTimeDelta)
{
	if (!m_bRoll && !m_iCombo) return;
	
	D3DXVec3Lerp(&m_vPos, &m_vPos, &m_vLerpPos, m_fLerp);

	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
}

void CPlayer::Charge(const _float& fTimeDelta)
{
	if (!m_fCharge) return;

	m_fCharge += fTimeDelta;
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
	if (!m_bAction)
	{
		m_bAction = true;
		m_eCurState = PS_ACTION;
		m_strFrameKey = L"intro_kneel";
	}
}

void CPlayer::Set_StopCrying()
{
	if (m_bAction)
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

	vector<CGameObject*> tempVec = CCollisionMgr::GetInstance()->Test_AABB(tAABB, CL_MONSTER | CL_GRASS);

	if (!tempVec.empty())
	{
		IMessageChannel::EVENT EAttack;
		EAttack.strType = L"Monster.Attacked";
		EAttack.eOBJID = Engine::OID_MONSTER;
		EAttack.hmapData.emplace(L"Attack", m_iAttack);
		EAttack.hmapData.emplace(L"Target", tempVec);
		m_pMessageChannel->Publish(EAttack);
	}
}

void CPlayer::Attacked(_int iDamage)
{
	if ((m_eCurState == PS_ROLL) || (m_fAcmlTime < PLAYER_INVINCIBLE_TIME)) return;

	if(m_iHp >= 2) m_iHp -= iDamage;	// 시연용

	if (m_eCurState == PS_HIT) return;

	if (m_eCurState == PS_CHARGE)
	{
		if (m_strFrameKey == L"charge-end") return;
		else m_fCharge = 0.f;
	}
	else if (m_eCurState == PS_ACTION)
	{ m_bAction = false; return; }

	m_eCurState = PS_HIT;
	m_fAcmlTime = 0.f;
}

void CPlayer::Update_Warp(const _float fTimeDelta)
{
	if (!m_IsWarp) { return; }

	if (m_fWarpDeleay >= 0.5f)
	{
		m_pTransformCom->Set_Pos(m_vWarpPos.x, 0.f, m_vWarpPos.z);
		if (m_fWarpDeleay >= 1.0f)
		{
			m_IsWarp = false;
			IMessageChannel::EVENT FadeEvent;
			FadeEvent.strType = L"Fade.Warp";
			FadeEvent.hmapData[L"Fade"] = wstring(L"FadeIn");
			m_pMessageChannel->Publish(FadeEvent);
			m_fWarpDeleay = 0.f;
			return;
		}
	}
	m_fWarpDeleay += fTimeDelta;

	
}

void	CPlayer::OnCollision(CGameObject* pObject)
{
	if (pObject->Get_OBJID() == OID_MONSTER)
	{
		//m_pTransformCom->Set_Pos(10.f, 10.f, 10.f);
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
		m_bCanTrigger = true;
		m_pTriggerPoint = static_cast<CTriggerPoint*>(pObject);
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
	CGameObject::Free();
}
