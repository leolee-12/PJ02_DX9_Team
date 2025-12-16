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
		m_fFrameSpeed(0.f)
{
	ZeroMemory(m_fFrameEnd, sizeof(m_fFrameEnd));
}

CPlayer::CPlayer(const CPlayer& rhs)
	:	CGameObject(rhs),
		m_ePreState(PS_END),
		m_eCurState(PS_IDLE),
		m_fFrame(0.f),
		m_fFrameSpeed(0.f)
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

	m_hmapSubHandles.insert({ L"StartGame.Move", m_pMessageChannel->Subscribe(L"Start_Game", [this](const IMessageChannel::EVENT& Event) {
		if (Event.eOBJID == this->Get_OBJID()) {
			m_pTransformCom->Set_Pos(10.f, 10.f, 10.f);
		}
		}) });

	

	//m_pTransformCom->m_vInfo[INFO_POS].y = 1.f;

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
	_vec3		vLook;

	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);

	if (GetAsyncKeyState(VK_UP))
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, 10.f);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -10.f);
	}
		

	if (GetAsyncKeyState(VK_LEFT))
	{
		m_pTransformCom->Rotation(ROT_Y, 180.f * fTimeDelta);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		m_pTransformCom->Rotation(ROT_Y, -180.f * fTimeDelta);
	}

	if (CDInputMgr::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80)
	{
		_vec3		vPickPos = Picking_OnTerrain();

		_vec3	vDir = vPickPos - m_pTransformCom->m_vInfo[INFO_POS];
		
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vDir, &vDir), fTimeDelta, 10.f);
	}

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

	// 지형 월드변환 추가로 인한 플레이어 좌표계 로컬로 끌어내리기
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

	switch (m_eCurState)
	{
	case PS_IDLE:
	{

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
