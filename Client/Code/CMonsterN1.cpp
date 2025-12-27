#include "pch.h"
#include "CMonsterN1.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include <CPersistentMgr.h>
#include "CN1_AI.h"

CMonsterN1::CMonsterN1(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CMonster(pGraphicDev),
		m_ePreState(N1S_END),
		m_eCurState(N1S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_fSpeed(0.f),
		m_iAttack(0)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CMonsterN1::CMonsterN1(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	:	CMonster(pGraphicDev, StageChannel),
		m_ePreState(N1S_END),
		m_eCurState(N1S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_fSpeed(0.f),
		m_iAttack(0)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}


CMonsterN1::CMonsterN1(const CMonsterN1& rhs)
	:	CMonster(rhs),
		m_ePreState(N1S_END),
		m_eCurState(N1S_SPAWN),
		m_fFrame(0.f),
		m_fFrameEnd(0.f),
		m_fFrameSpeed(0.f),
		m_fSpeed(rhs.m_fSpeed),
		m_iAttack(rhs.m_iAttack)
{
}

CMonsterN1::~CMonsterN1()
{
}

HRESULT CMonsterN1::Ready_GameObject()
{
	m_eOBJID = OID_MONSTER;
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pAICom->Set_OwnerTransform(m_pTransformCom);
	m_pAICom->Set_State<MONSTER_N1_STATE>(N1S_SPAWN);
	
	Ready_Variable();

	Ready_Event();

	return S_OK;
}

_int CMonsterN1::Update_GameObject(const _float& fTimeDelta)
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

	return iExit;
}

void CMonsterN1::LateUpdate_GameObject(const _float& fTimeDelta)
{
	//m_pTarget = CPersistentMgr::GetInstance()->Get_PlayerTransform();
	//
	//if (m_pTarget != nullptr)
	//{
	//	_vec3 vTargetPos;
	//	m_pTarget->Get_Info(INFO_POS, &vTargetPos);
	//	m_vDir = vTargetPos - m_vPos;
	//	D3DXVec3Normalize(&m_vDir, &m_vDir);
	//	m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
	//	m_eCurState = N1S_RUN;
	//}
	//else m_eCurState = N1S_IDLE;

	Update_State();

	Check_Frame();
	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMonsterN1::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	
	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

HRESULT CMonsterN1::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MonsterN1Texture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	// AI
	pComponent = m_pAICom = dynamic_cast<CN1_AI*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_N1_AI"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_AI", pComponent });

	return S_OK;
}

void CMonsterN1::Ready_Variable()
{
	m_pColliderCom->RegisterToManager(this, CL_MONSTER);

	m_pTransformCom->Set_Pos(_float(rand() % 20), 1.f, _float(rand() % 20));
	m_pTransformCom->Set_Scale(3.f, 3.f, 3.f);
	m_fFrameSpeed = 24.f;

	_float fAngle(0.f);

	for (_uint i = 0; i < DIR_END; ++i)
	{
		m_vNormDir[i] = { cosf(fAngle), 0.f, -sinf(fAngle) };
		fAngle += D3DX_PI * 0.25f;
	}

	m_vDir = m_vNormDir[DIR_LEFT];
	D3DXMatrixIdentity(&m_matTex);

	m_fSpeed = 1.f;
	m_iAttack = 1;
}

void CMonsterN1::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Monster_Damaged", m_pMessageChannel->Subscribe(L"Monster.Attacked", [this](const IMessageChannel::EVENT& Event) {
	for (auto& Target : any_cast<vector<CGameObject*>>(Event.hmapData.find(L"Target")->second))
	{
		if (Target == this)
		{
			m_iHp -= any_cast<_int>(Event.hmapData.find(L"Attack")->second);
		}
	}
	}) });
}

void CMonsterN1::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case N1S_IDLE:
	{
		m_fFrameEnd = 24.f;
	}
	break;

	case N1S_RUN:
	{
		m_fFrameEnd = 14.f;
	}
	break;

	case N1S_ATTACK:
	{
		m_fFrameEnd = 18.f;
	}
	break;

	case N1S_HIT:
	{
		m_fFrameEnd = 11.f;
	}
	break;

	case N1S_SPAWN:
	{
		m_fFrameEnd = 36.f;
	}
	break;

	case N1S_JEER:
	{
		m_fFrameEnd = 48.f;
	}
	break;

	case N1S_PRAY:
	{
		m_fFrameEnd = 32.f;
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CMonsterN1::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		if ((m_eCurState == N1S_SPAWN) || (m_eCurState == N1S_HIT))
		{
			m_eCurState = N1S_IDLE;
		}
		else if (m_eCurState == N1S_ATTACK)
		{
			if (m_bAttack) m_eCurState = N1S_IDLE;
			else
			{
				m_bAttack = true;
				// 공격 메시지 발송
			}
		}
	}
}

void CMonsterN1::Set_Texture()
{
	_uint iFrame = m_fFrame;

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.25f;	// 세로는 4칸 고정(MonsterN1)

	switch (m_eCurState)
	{
	case N1S_IDLE:
	{
		//if (m_vDir == m_vNormDir[DIR_UP] || m_vDir == m_vNormDir[DIR_LU] || m_vDir == m_vNormDir[DIR_RU]) iU += 2;
	}
	break;

	case N1S_RUN:
	{
	}
	break;

	case N1S_ATTACK:
	{
	}
	break;

	case N1S_HIT:
	{
	}
	break;

	case N1S_SPAWN:
	{
	}
	break;

	case N1S_JEER:
	{
	}
	break;

	case N1S_PRAY:
	{
	}
	break;
	}

	m_matTex._31 = _float(iU) * 0.0625f;
	m_matTex._32 = _float(iV) * 0.25f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(_uint(m_eCurState));
}

void CMonsterN1::Update_State()
{
	if (m_eCurState == N1S_SPAWN || m_eCurState == N1S_HIT)
		return;

	if (m_eCurState == N1S_ATTACK && m_fFrame < m_fFrameEnd)
		return;

	m_eCurState = m_pAICom->Get_RecommendState<MONSTER_N1_STATE>();
}

CMonsterN1* CMonsterN1::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
	CMonsterN1* pMonster = new CMonsterN1(pGraphicDev, StageChannel);

	if (FAILED(pMonster->Ready_GameObject()))
	{
		Safe_Release(pMonster);
		MSG_BOX("pMonster Create Failed");
		return nullptr;
	}

	return pMonster;
}

void CMonsterN1::Free()
{
	CGameObject::Free();
}
