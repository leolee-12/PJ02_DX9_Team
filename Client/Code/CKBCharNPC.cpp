#include "pch.h"
#include "CKBCharNPC.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CKBCharNPC::CKBCharNPC(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_ePreState(KBC_END)
	, m_eCurState(KBC_IDLE)
	, m_fFrame(0.f)
	, m_fFrameEnd(0.f)
	, m_fFrameSpeed(0.f)
{
	ZeroMemory(&m_vScreenPos, sizeof(_vec3));
	D3DXMatrixIdentity(&m_matTex);
}

CKBCharNPC::~CKBCharNPC()
{
}

HRESULT CKBCharNPC::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();

	return S_OK;
}

_int CKBCharNPC::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
		return iExit;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CKBCharNPC::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_Frame();

	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CKBCharNPC::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CKBCharNPC::OnCollision(CGameObject* pObject)
{
}

HRESULT CKBCharNPC::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTex
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_KBCharNPC"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CKBCharNPC::Ready_Variable()
{
	_vec3 fScale = { 300.f, 300.f, 1.f };

	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->Set_Scale(fScale.x, fScale.y, fScale.z);

	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

void CKBCharNPC::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case KBC_IDLE:
		m_fFrameEnd = 64.f;
		break;
	case KBC_PLAY_DICE:
		m_fFrameEnd = 32.f;
		break;
	case KBC_TAKE_DICE:
		m_fFrameEnd = 32.f;
		break;
	case KBC_WIN_GAME:
		m_fFrameEnd = 28.f;
		break;
	case KBC_WIN_GAME_LOOP:
		m_fFrameEnd = 32.f;
		break;
	case KBC_LOSE_DICE:
		m_fFrameEnd = 32.f;
		break;
	case KBC_LOSE_GAME:
		m_fFrameEnd = 32.f;
		break;
	case KBC_LOSE_GAME_LOOP:
		m_fFrameEnd = 48.f;
		break;
	}

	m_ePreState = m_eCurState;
}

void CKBCharNPC::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		switch (m_eCurState)
		{
		case KBC_IDLE:
			m_fFrame = 0.f;
			break;
		case KBC_PLAY_DICE:
			m_eCurState = KBC_IDLE;
			break;
		case KBC_TAKE_DICE:
			m_eCurState = KBC_IDLE;
			break;
		case KBC_WIN_GAME:
			m_eCurState = KBC_WIN_GAME_LOOP;
			break;
		case KBC_WIN_GAME_LOOP:
			m_fFrame = 0.f;
			break;
		case KBC_LOSE_DICE:
			m_eCurState = KBC_IDLE;
			break;
		case KBC_LOSE_GAME:
			m_eCurState = KBC_LOSE_GAME_LOOP;
			break;
		case KBC_LOSE_GAME_LOOP:
			m_fFrame = 0.f;
			break;
		}
	}
}

void CKBCharNPC::Set_Texture()
{
	_uint iFrame = _uint(m_fFrame);
	D3DXMatrixIdentity(&m_matTex);

	// 16x4 그리드
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;		// 1/16 (16열)
	m_matTex._22 = 0.25f;		// 1/4 (4행)
	m_matTex._31 = _float(iU) * 0.0625f;
	m_matTex._32 = _float(iV) * 0.25f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);
	m_pTextureCom->Set_Texture(_uint(m_eCurState));
}

CKBCharNPC* CKBCharNPC::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos)
{
	CKBCharNPC* pInstance = new CKBCharNPC(pGraphicDev);

	pInstance->m_vPos = vPos;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CKBCharNPC Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CKBCharNPC::Free()
{
	CUi::Free();
}
