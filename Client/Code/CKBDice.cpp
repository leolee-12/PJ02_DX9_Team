#include "pch.h"
#include "CKBDice.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSoundMgr.h"

CKBDice::CKBDice(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_iValue(1)
	, m_eState(DS_IDLE)
	, m_fRollTime(0.f)
	, m_fRollDuration(0.5f)
	, m_fRollInterval(0.1f)
	, m_fRollAccum(0.f)
	, m_fMoveTime(0.f)
	, m_fMoveDuration(1.f)
	, m_bDead(false)
	, m_fBounceAccel(0.f)
	, m_fBounceSpeed(0.f)
	, m_fBounceDemp(0.8f)
	, m_iTurn(0)
	, m_fBounceGoal(0.f)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
	ZeroMemory(&m_vStartPos, sizeof(_vec3));
	ZeroMemory(&m_vTargetPos, sizeof(_vec3));
}

CKBDice::~CKBDice()
{
}

HRESULT CKBDice::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(50.f, 50.f, 1.f);
	switch (m_iTurn)
	{
	case 0: // 플레이어
		m_vPos = _vec3((_float(-WINCX) * 0.5f) + 220.f, _float(-WINCY / 2) + 140.f, 0.f);
		m_fBounceAccel = 9.8f;
		m_fBounceGoal = m_vPos.x + 50.f;
		break;
	case 1: // NPC
		m_vPos = _vec3((_float(WINCX) * 0.5f) - 220.f, _float(WINCY / 2) - 140.f, 0.f);
		m_fBounceAccel = -9.8f;
		m_fBounceGoal = m_vPos.x - 50.f;
		break;
	}

	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	Roll();

	return S_OK;
}

_int CKBDice::Update_GameObject(const _float& fTimeDelta)
{
	// 삭제 플래그가 설정되면 DEAD 반환
	if (m_bDead)
		return DEAD;

	_int iExit = CUi::Update_GameObject(fTimeDelta);

	switch (m_eState)
	{
	case DS_ROLLING:
		Update_Rolling(fTimeDelta);
		break;
	case DS_MOVING:
		Update_Moving(fTimeDelta);
		break;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CKBDice::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CUi::LateUpdate_GameObject(fTimeDelta);
	//m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CKBDice::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	// m_iValue is 1~6, texture index is 0~5
	m_pTextureCom->Set_Texture(m_iValue - 1);

	m_pBufferCom->Render_Buffer();
}

void CKBDice::OnCollision(CGameObject* pObject)
{
}

void CKBDice::Roll()
{
	m_eState = DS_ROLLING;
	Set_Rolling_Motion();
	m_fRollTime = 0.f;
	m_fRollAccum = 0.f;
	wchar_t szSound[32];
	swprintf_s(szSound, L"kb_dice_%d.wav", Get_Rand_Int(1, 8));
	CSoundMgr::GetInstance()->Play(szSound, CHANNELID::SOUND_EFFECT, 1.f);
}

void CKBDice::ShowResult(_int iFinalValue)
{
	m_iValue = iFinalValue;
	m_eState = DS_SHOWING;
}

void CKBDice::MoveTo(const _vec3& vTargetPos)
{
	m_pTransformCom->Get_Info(INFO_POS, &m_vStartPos);
	m_vTargetPos = vTargetPos;
	m_fMoveTime = 0.f;
	m_eState = DS_MOVING;
}

void CKBDice::SetPosition(const _vec3& vPos)
{
	m_vPos = vPos;
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
}

void CKBDice::Update_Rolling(const _float& fTimeDelta)
{
	m_fRollTime += fTimeDelta;
	m_fRollAccum += fTimeDelta;

	// Change texture at interval (0.1 sec)
	if (m_fRollAccum >= m_fRollInterval)
	{
		m_fRollAccum = 0.f;
		m_iValue = Engine::Get_Rand_Int(1, 6);
	}

	// Auto stop after duration (2 sec)
	if (m_fRollTime >= m_fRollDuration)
	{
		m_iValue = Engine::Get_Rand_Int(1, 6);
		m_eState = DS_SHOWING;
	}
	Rolling_Motion(fTimeDelta);
}

void CKBDice::Update_Moving(const _float& fTimeDelta)
{
	m_fMoveTime += fTimeDelta;

	_float fRatio = m_fMoveTime / m_fMoveDuration;

	if (fRatio >= 1.f)
	{
		fRatio = 1.f;
		m_eState = DS_IDLE;
	}

	D3DXVec3Lerp(&m_vStartPos, &m_vStartPos, &m_vTargetPos, fRatio);

	m_pTransformCom->Set_Pos(m_vStartPos.x, m_vStartPos.y, m_vStartPos.z);
}

void CKBDice::Set_Rolling_Motion()
{
	m_vStartPos = m_vPos;
	m_vTargetPos = m_vStartPos;
	switch (m_iTurn)
	{
		case 0: // 플레이어
			m_vTargetPos.x += 50.f;
			break;
		case 1: // NPC
			m_vTargetPos.x -= 50.f;
			break;
	}
}

void CKBDice::Rolling_Motion(const _float& fTimeDelta)
{
	m_fBounceSpeed += m_fBounceAccel * fTimeDelta * 8.f;
	m_vPos.x += m_fBounceSpeed;
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	switch (m_iTurn)
	{
		case 0: // 플레이어
		if (m_vPos.x >= m_fBounceGoal)
		{
			m_vPos.x = m_fBounceGoal;
			m_fBounceSpeed = -m_fBounceSpeed * m_fBounceDemp;
		}
		break;
		case 1: // NPC
		if (m_vPos.x <= m_fBounceGoal)
		{
			m_vPos.x = m_fBounceGoal;
			m_fBounceSpeed = -m_fBounceSpeed * m_fBounceDemp;
		}
		break;
	}

	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
	//m_pTransformCom->Update_Component(fTimeDelta);
}

HRESULT CKBDice::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// Buffer
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_KBDice"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CKBDice* CKBDice::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iTurn)
{
	CKBDice* pDice = new CKBDice(pGraphicDev);
	pDice->m_iTurn = iTurn;

	if (FAILED(pDice->Ready_GameObject()))
	{
		Safe_Release(pDice);
		MSG_BOX("CKBDice Create Failed");
		return nullptr;
	}

	return pDice;
}

void CKBDice::Free()
{
	CUi::Free();
}
