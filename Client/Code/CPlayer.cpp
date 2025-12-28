#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CCollisionMgr.h"

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
	m_fRollSpeed(5.f),
	m_fCharge(0.f),
	m_fChargeMax(3.f)
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
		m_fRollSpeed(5.f),
		m_fCharge(0.f),
		m_fChargeMax(3.f)
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
		m_fLerp(rhs.m_fRollSpeed),
		m_fRollSpeed(rhs.m_fRollSpeed),
		m_fCharge(0.f),
		m_fChargeMax(rhs.m_fChargeMax)
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
	Move_Frame(fTimeDelta);

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	
	Set_OnTerrain();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);
	Move_Roll(fTimeDelta);
	Charge(fTimeDelta);
	Check_Frame();
	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

#pragma region 기존 (반전X)

	//Set_TextureSet();
	//m_pBufferCom->Render_Buffer();

#pragma endregion

#pragma region 스프라이트 반전 도입 - 버텍스/인덱스 버퍼 미사용

	Set_TextureSet();
	// 8방향 모두 스프라이트 이미지는 비효율적 -> 좌우반전해서 사용
	bool bFlipH = (m_vDir == m_vNormDir[DIR_RU] || m_vDir == m_vNormDir[DIR_RIGHT] || m_vDir == m_vNormDir[DIR_RD]);
	_float u1 = bFlipH ? 1.f : 0.f;
	_float u2 = bFlipH ? 0.f : 1.f;
	
	// bFlipH = false (반전X)
	//(u,v) : (0.f, 1.f)	|	(1.f, 1.f)
	//(u,v) : (0.f, 0.f)	|	(1.f, 0.f)
	
	// bFlipH = true (반전)
	//(u,v) : (1.f, 1.f)	|	(0.f, 1.f)
	//(u,v) : (1.f, 0.f)	|	(0.f, 0.f)
	
	// 문제는 버텍스 버퍼를 동적으로 수정하면 오버헤드 심함
	// - 버텍스/인덱스 버퍼를 사용하지 않고 그리기 : DrawPrimitiveUP
	// - 인덱스버퍼 없으므로 D3DPT_TRIANGLELIST 사용 불가 : D3DPT_TRIANGLESTRIP으로 그리면 (0-1-2), (2-1-3) 으로 그려짐
	// 0 1 > 0 1
	// 3 2 > 2 3
	
	VTXTEX pVertex[4];
	
	pVertex[0].vPosition = { -1.f, 1.f, 0.f };
	pVertex[0].vTexUV = { u1, 0.f };
	
	pVertex[1].vPosition = { 1.f, 1.f, 0.f };
	pVertex[1].vTexUV = { u2, 0.f };
	
	pVertex[3].vPosition = { 1.f, -1.f, 0.f };
	pVertex[3].vTexUV = { u2, 1.f };
	
	pVertex[2].vPosition = { -1.f, -1.f, 0.f };
	pVertex[2].vTexUV = { u1, 1.f };
	
	m_pGraphicDev->SetFVF(Engine::FVF_TEX);
	
	m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(VTXTEX));
	
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

#pragma endregion

#pragma region 스프라이트 시트

	//m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	//Set_Texture();
	//m_pBufferCom->Render_Buffer();
	//m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

#pragma endregion
}

void CPlayer::Ready_Variable()
{
	m_pColliderCom->RegisterToManager(this, CL_PLAYER);

	m_eOBJID = OID_PLAYER;

	m_pTransformCom->Set_Pos(10.f, 0.f, 10.f);
	m_pTransformCom->Set_Scale(5.f, 5.f, 5.f);
	//m_pTransformCom->Set_Scale(7.f, 7.f, 7.f);	// Tex2
	m_fFrameSpeed = 24.f;

	_float fAngle(0.f);

	for (_uint i = 0; i < DIR_END; ++i)
	{
		m_vNormDir[i] = { cosf(fAngle), 0.f, -sinf(fAngle) };
		fAngle += D3DX_PI * 0.25f;
	}

	m_vDir = m_vNormDir[DIR_LEFT];
	m_fSpeed = 10.f;
	m_iAttack = 1;
}

void CPlayer::Ready_Event()
{
	//m_hmapSubHandles.insert({ L"StartGame.Move", m_pMessageChannel->Subscribe(L"Start_Game", [this](const IMessageChannel::EVENT& Event) {
	//	if (Event.eOBJID == this->Get_OBJID()) {
	//		m_pTransformCom->Set_Pos(10.f, 10.f, 10.f);
	//	}
	//	}) });
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

	// Texture
	//pComponent = m_pTextureCom2 = dynamic_cast<Engine::CTexture*>
	//	(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlayerTexture2"));
	//
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//
	//m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

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
	if (!m_bRoll && !m_iCombo && !m_fCharge)
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
			m_eCurState = PS_IDLE;
		}
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_SPACE))
	{
		if ((m_bRoll) || (m_iCombo) || (m_fCharge)) return;

		m_bRoll = true;
		m_eCurState = PS_ROLL;
		m_vRollPos = m_vPos + m_vDir * 5.f;
	}

	if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)	// 눌렀을 때 한 번만 true
	{			
		if ((m_iCombo == 3) || (m_fCharge)) return;

		m_iCombo++;
		m_bRoll = false;
		m_fFrame = 0.f;
		m_eCurState = PS_ATTACK;
		m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed - 5.f);
		HitBox();
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

	//if (CDInputMgr::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80)
	//{
	//	_vec3		vPickPos = Picking_OnTerrain();

	//	_vec3	vDir = vPickPos - m_pTransformCom->m_vInfo[INFO_POS];
	//	
	//	m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vDir, &vDir), fTimeDelta, 10.f);
	//}

	if (GetAsyncKeyState('P'))
	{
		m_iHp = 0;
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
		m_fFrameEnd = _float(m_pTextureCom->Get_TextureEnd(L"idle"));
	}
	break;

	case PS_RUN:
	{
		m_fFrameEnd = m_pTextureCom->Get_TextureEnd(L"run-up");
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
	}

	m_ePreState = m_eCurState;
}

void CPlayer::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame > m_fFrameEnd)
	{
		m_fFrame = 0.f;
		
		// 구르기 및 공격은 프레임이 끝날 때까지 유지되었다가 종료
		if (m_eCurState == PS_ROLL)
		{
			m_bRoll = false;
		}
		else if (m_eCurState == PS_ATTACK)
		{
			m_iCombo = 0;
		}
		else if (m_eCurState == PS_CHARGE)
		{
			if (m_strFrameKey == L"charge-start") m_strFrameKey = L"charge-loop";
			else if (m_strFrameKey == L"charge-end")
			{
				m_fCharge = 0.f;
				m_fChargeMax = 3.f;
			}
		}
	}
}

void CPlayer::Set_Texture()
{
	_uint iFrame = m_fFrame;

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.0625f;	// 세로는 16칸 고정(Player)

	switch (m_eCurState)
	{
	case PS_IDLE:
	{
		//if (m_vDir == m_vNormDir[DIR_UP] || m_vDir == m_vNormDir[DIR_LU] || m_vDir == m_vNormDir[DIR_RU]) iU += 2;
	}
	break;

	case PS_RUN:
	{
	}
	break;

	case PS_ROLL:
	{
	}
	break;

	case PS_ATTACK:
	{
	}
	break;

	case PS_CHARGE:
	{
	}
	break;

	case PS_ACTION:
	{
	}
	break;
	}

	m_matTex._31 = _float(iU) * 0.0625f;
	m_matTex._32 = _float(iV) * 0.0625f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	//m_pTextureCom2->Set_Texture(_uint(m_eCurState));
}

void CPlayer::Set_TextureSet()
{
	wstring strPreKey = m_strFrameKey;

	switch (m_eCurState)
	{
	case PS_IDLE:
	{
		if (m_vDir == m_vNormDir[DIR_UP] || m_vDir == m_vNormDir[DIR_LU] || m_vDir == m_vNormDir[DIR_RU] ) m_strFrameKey = L"idle-up";
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
		if		(m_vDir == m_vNormDir[DIR_UP]	|| m_vDir == m_vNormDir[DIR_LU]	|| m_vDir == m_vNormDir[DIR_RU])	m_strFrameKey = L"roll-up";
		else if (m_vDir == m_vNormDir[DIR_DOWN]	|| m_vDir == m_vNormDir[DIR_LD]	|| m_vDir == m_vNormDir[DIR_RD])	m_strFrameKey = L"roll-down";
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

	case PS_CHARGE:
	{
		// 키인풋 & Move_Frame에서 관리
	}
	break;
	}

	if (m_strFrameKey != strPreKey) m_fFrame = 0.f;

	m_fFrameEnd = m_pTextureCom->Get_TextureEnd(m_strFrameKey); 

	m_pTextureCom->Set_Texture(m_strFrameKey, _uint(m_fFrame));
}

void CPlayer::Move_Roll(const _float& fTimeDelta)
{
	if (!m_bRoll) return;
	
	D3DXVec3Lerp(&m_vPos, &m_vPos, &m_vRollPos, m_fLerp);

	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
}

void CPlayer::Charge(const _float& fTimeDelta)
{
	if (!m_fCharge) return;

	m_fCharge += fTimeDelta;
}

void CPlayer::HitBox()
{
	AABB tAABB = { m_vPos.x, m_vPos.y, m_vPos.z,
					2.f, 1.f, 2.f };

	vector<CGameObject*> tempVec = CCollisionMgr::GetInstance()->Test_AABB(tAABB, CL_MONSTER);

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

void	CPlayer::OnCollision(CGameObject* pObject)
{
	if (pObject->Get_OBJID() == OID_MONSTER)
	{
		m_pTransformCom->Set_Pos(10.f, 10.f, 10.f);
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
