#include "pch.h"
#include "CKBCharPlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CKBCharPlayer::CKBCharPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_ePreState(KBC_END)
	, m_eCurState(KBC_IDLE)
	, m_fFrame(0.f)
	, m_fFrameEnd(0.f)
	, m_fFrameSpeed(0.f)
	, m_bFlipX(true)
{
}

CKBCharPlayer::~CKBCharPlayer()
{
}

HRESULT CKBCharPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();

	return S_OK;
}

_int CKBCharPlayer::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
		return iExit;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CKBCharPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_Frame();

	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CKBCharPlayer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CKBCharPlayer::OnCollision(CGameObject* pObject)
{
}

HRESULT CKBCharPlayer::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_KBCharPlayer"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CKBCharPlayer::Ready_Variable()
{
	_vec3 fScale = { 300.f, 300.f, 1.f };

	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->Set_Scale(fScale.x, fScale.y, fScale.z);

	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

void CKBCharPlayer::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case KBC_IDLE:
		m_fFrameEnd = 96.f;
		break;
	case KBC_PLAY_DICE:
		m_fFrameEnd = 32.f;
		break;
	case KBC_TAKE_DICE:
		m_fFrameEnd = 36.f;
		break;
	case KBC_WIN_GAME:
		m_fFrameEnd = 52.f;
		break;
	case KBC_WIN_GAME_LOOP:
		m_fFrameEnd = 72.f;
		break;
	case KBC_LOSE_DICE:
		m_fFrameEnd = 58.f;
		break;
	case KBC_LOSE_GAME:
		m_fFrameEnd = 64.f;
		break;
	case KBC_LOSE_GAME_LOOP:
		m_fFrameEnd = 64.f;
		break;
	}

	m_ePreState = m_eCurState;
}

void CKBCharPlayer::Move_Frame(const _float& fTimeDelta)
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

void CKBCharPlayer::Set_Texture()
{
	_uint iFrame = _uint(m_fFrame);
	D3DXMatrixIdentity(&m_matTex);

	// 16x8 그리드
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;		// 1/16 (16열)
	m_matTex._22 = 0.125f;		// 1/8 (8행)

	if (m_bFlipX)
	{
		m_matTex._11 *= -1.f;
		m_matTex._31 = _float(iU + 1) * 0.0625f;	// 반전 시 오른쪽에서 왼쪽으로
	}
	else
	{
		m_matTex._31 = _float(iU) * 0.0625f;
	}
	m_matTex._32 = _float(iV) * 0.125f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);
	m_pTextureCom->Set_Texture(_uint(m_eCurState));
}

CKBCharPlayer* CKBCharPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos)
{
	CKBCharPlayer* pInstance = new CKBCharPlayer(pGraphicDev);

	pInstance->m_vPos = vPos;

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CKBCharPlayer Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CKBCharPlayer::Free()
{
	CUi::Free();
}
