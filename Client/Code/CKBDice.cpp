#include "pch.h"
#include "CKBDice.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CKBDice::CKBDice(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_iValue(1)
	, m_eState(DS_IDLE)
	, m_fRollTime(0.f)
	, m_fRollDuration(1.f)
	, m_fRollInterval(0.1f)
	, m_fRollAccum(0.f)
	, m_fMoveTime(0.f)
	, m_fMoveDuration(1.f)
	, m_bDead(false)
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
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	Roll();

	return S_OK;
}

_int CKBDice::Update_GameObject(const _float& fTimeDelta)
{
	// 삭제 플래그가 설정되면 DEAD 반환
	if (m_bDead)
		return DEAD;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

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
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
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
	m_fRollTime = 0.f;
	m_fRollAccum = 0.f;
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

CKBDice* CKBDice::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos)
{
	CKBDice* pDice = new CKBDice(pGraphicDev);
	pDice->m_vPos = vPos;

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
