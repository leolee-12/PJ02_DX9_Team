#include "pch.h"
#include "CTarotCard.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN1_AI.h"
#include "CSpeechBubble.h"
#include "CFontUI.h"
#include "CSoundMgr.h"
#include "CTarotInfo.h"

CTarotCard::CTarotCard(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CTarotCard::CTarotCard(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CUi(pGraphicDev, StageChannel),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}


CTarotCard::CTarotCard(const CTarotCard& rhs)
	: CUi(rhs),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CTarotCard::~CTarotCard()
{
}

HRESULT CTarotCard::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	return S_OK;
}

_int CTarotCard::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	Check_CusorColl();

	m_pTarotInfo->Update_GameObject(fTimeDelta);
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		return iExit;
	}

	if (!m_bWait) {
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CTarotCard::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_Frame();

	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);

	m_pTarotInfo->LateUpdate_GameObject(fTimeDelta);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTarotCard::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CTarotCard::OnCollision(CGameObject* pObject)
{
}

HRESULT CTarotCard::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcCol
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TarotCard"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CTarotCard::Ready_Variable()
{
	_vec3 fScale = { 1000.f, 1000.f, 1.f };
	m_vPos = { m_vScreenPos.x - _float(WINCX / 2), -m_vScreenPos.y + _float(WINCY / 2), m_vScreenPos.z };
	// Transform 세팅
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->Set_Scale(fScale.x, fScale.y, fScale.z);

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);

	_vec2 vInfoPos = { m_vPos.x + 200.f, m_vPos.y };
	
	m_pTarotInfo = CTarotInfo::Create(m_pGraphicDev, vInfoPos, _uint(m_eType));

	NULL_CHECK_MSG(m_pTarotInfo, L"타로인포 생성 실패.");
}

void CTarotCard::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Tarot.Selected", m_pMessageChannel->Subscribe(L"Tarot.Selected", [this](const IMessageChannel::EVENT& Event)
		{
			m_iHp = 0;
		}
	) });
}

void CTarotCard::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case TCS_INTRO:
	{
		m_fFrameEnd = 16.f;
	}
	break;
	case TCS_INTRO2:
	{
		m_fFrameEnd = 13.f;
		m_eType = TAROTCARD_TYPE(_int(m_eType) + 1);
	}
	break;

	case TCS_SHOWING:
	{
		m_fFrame = 13.f;
		m_fFrameEnd = 14.f;
		m_pTarotInfo->Active();
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CTarotCard::Move_Frame(const _float& fTimeDelta)
{
	if (m_bWait) { return; }

	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		IMessageChannel::EVENT RatauEvent;

		switch (m_eCurState)
		{
		case TCS_INTRO:
		{
			m_eCurState = TCS_INTRO2;
		}
		break;

		case TCS_INTRO2:
		{
			m_eCurState = TCS_SHOWING;
		}
		break;

		case TCS_SHOWING:
		{
			m_fFrame = m_fFrameEnd - 0.01f;
		}
		break;
		}
	}
}

void CTarotCard::Set_Texture()
{
	//_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 4;
	_uint iV = iFrame / 4;

	m_matTex._11 = 0.25f;	// 가로는 8칸 고정
	m_matTex._22 = 0.25f;	// 세로는 8칸 고정(Ratau)

	switch (m_eCurState)
	{
	case TCS_INTRO:
		break;

	case TCS_SHOWING:
		break;
	}

	//if (bFilpX)
	//{
	//	m_matTex._11 *= -1.f;
	//	m_matTex._31 = _float(iU + 1) * 0.125f;	// 반전 O : 오른쪽에서 왼쪽으로 읽음
	//}
	//else
	//{
	m_matTex._31 = _float(iU) * 0.25f;	// 반전 X : 왼쪽에서 오른쪽으로 읽음
	//}

	m_matTex._32 = _float(iV) * 0.25f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(_uint(m_eType));
}

void CTarotCard::Check_CusorColl()
{
	if (m_eCurState != TCS_SHOWING) { return; }

	POINT pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	const float fScreenCenterX = _float(WINCX / 2);
	const float fScreenCenterY = _float(WINCY / 2);

	RECT tRc = {
		_long(m_vScreenPos.x - m_vHitHalfScale.x),
		_long(m_vScreenPos.y - m_vHitHalfScale.y),
		_long(m_vScreenPos.x + m_vHitHalfScale.x),
		_long(m_vScreenPos.y + m_vHitHalfScale.y)
	};


	if (PtInRect(&tRc, pt))
	{
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			IMessageChannel::EVENT TarotEvent;
			TarotEvent.strType = L"Tarot.Selected";
			TarotEvent.hmapData[L"TarotType"] = _uint(m_eType);
			m_pMessageChannel->Publish(TarotEvent);
			CSoundMgr::GetInstance()->Play(L"TarotCardClose.wav", SOUND_EFFECT, 0.2f);
		}
		m_pTransformCom->Set_Scale(1100.f, 1100.f, 1.f);
	}
	else {
		m_pTransformCom->Set_Scale(1000.f, 1000.f, 1.f);
	}
}

CTarotCard* CTarotCard::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos, TAROTCARD_TYPE eType)
{
	CTarotCard* pTarotCard = new CTarotCard(pGraphicDev, StageChannel);

	pTarotCard->m_vScreenPos = vPos;
	pTarotCard->m_eType = eType;

	if (FAILED(pTarotCard->Ready_GameObject()))
	{
		Safe_Release(pTarotCard);
		MSG_BOX("pTarotCard Create Failed");
		return nullptr;
	}


	return pTarotCard;
}

void CTarotCard::Free()
{
	Safe_Release(m_pTarotInfo);
	CGameObject::Free();
}
