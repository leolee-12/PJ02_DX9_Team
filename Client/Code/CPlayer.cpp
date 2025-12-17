#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	:	CGameObject(pGraphicDev, StageChannel),
		m_ePreState(PS_END),
		m_eCurState(PS_IDLE),
		m_fFrame(0.f),
		m_fFrameSpeed(0.f),
		m_fSpeed(0.f),
		m_iAttack(0)
{
	ZeroMemory(m_fFrameEnd, sizeof(m_fFrameEnd));
}

CPlayer::CPlayer(const CPlayer& rhs)
	:	CGameObject(rhs),
		m_ePreState(PS_END),
		m_eCurState(PS_IDLE),
		m_fFrame(0.f),
		m_fFrameSpeed(0.f),
		m_fSpeed(rhs.m_fSpeed),
		m_iAttack(rhs.m_iAttack),
		m_vPos(rhs.m_vPos)
{
	memcpy(m_fFrameEnd, rhs.m_fFrameEnd, sizeof(m_fFrameEnd));
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eOBJID = OID_PLAYER;

	//m_hmapSubHandles.insert({ L"StartGame.Move", m_pMessageChannel->Subscribe(L"Start_Game", [this](const IMessageChannel::EVENT& Event) {
	//	if (Event.eOBJID == this->Get_OBJID()) {
	//		m_pTransformCom->Set_Pos(10.f, 10.f, 10.f);
	//	}
	//	}) });
	m_pTransformCom->Set_Pos(10.f, 0.f, 10.f);
	m_fFrameSpeed = 90.f;

	_float fAngle(0.f);

	for (_uint i = 0; i < DIR_END; ++i)
	{
		m_vNormDir[i] = { cosf(fAngle), 0.f, -sinf(fAngle) };
		fAngle += D3DX_PI * 0.25f;
	}

	m_fSpeed = 10.f;
	m_iAttack = 1;

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	Check_Frame();
	Move_Frame(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	
	Set_OnTerrain();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);
	Compute_BillBoard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);


	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		
	m_pTextureCom->Set_Texture(_uint(m_fFrame));
	
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlayerTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Calculator"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });


	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	_vec3	vDir;

	if (GetAsyncKeyState('W'))
	{
		if (GetAsyncKeyState('A')) vDir = m_vNormDir[DIR_LU];

		else if (GetAsyncKeyState('D')) vDir = m_vNormDir[DIR_RU];

		else vDir = m_vNormDir[DIR_UP];

		m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed);
	}

	else if (GetAsyncKeyState('S'))
	{
		if (GetAsyncKeyState('A')) vDir = m_vNormDir[DIR_LD];

		else if (GetAsyncKeyState('D')) vDir = m_vNormDir[DIR_RD];

		else vDir = m_vNormDir[DIR_DOWN];

		m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed);
	}

	else if (GetAsyncKeyState('A'))
	{
		vDir = m_vNormDir[DIR_LEFT];
		m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed);
	}

	else if (GetAsyncKeyState('D'))
	{
		vDir = m_vNormDir[DIR_RIGHT];
		m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed);
	}

	else
	{
		m_eCurState = PS_IDLE;
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

	// ���� ���庯ȯ �߰��� ���� �÷��̾� ��ǥ�� ���÷� �������
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
		m_fFrameEnd[PS_IDLE] = 180;
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CPlayer::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame > m_fFrameEnd[m_eCurState])
		m_fFrame = 0.f;
}

void CPlayer::BillBoard(ROTATION eAxis)
{
	_matrix matWorld, matView, matBill;

	matWorld = *m_pTransformCom->Get_World();
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);

	switch (eAxis)
	{
	case ROT_X:
		matBill._22 = matView._22;
		matBill._23 = matView._23;
		matBill._32 = matView._32;
		matBill._33 = matView._33;
		break;

	case ROT_Y:
		matBill._11 = matView._11;
		matBill._13 = matView._13;
		matBill._31 = matView._31;
		matBill._33 = matView._33;
		break;

	case ROT_Z:
		matBill._11 = matView._11;
		matBill._12 = matView._12;
		matBill._21 = matView._21;
		matBill._22 = matView._22;
		break;
	}

	D3DXMatrixInverse(&matBill, 0, &matBill);

	matWorld = matBill * matWorld;

	m_pTransformCom->m_matWorld = matWorld;
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

void CPlayer::Free()
{
	Safe_Release(m_pBufferCom);
	
	CGameObject::Free();
}
