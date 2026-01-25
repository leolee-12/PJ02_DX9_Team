#include "pch.h"
#include "CTarotSeller.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN1_AI.h"
#include "CSpeechBubble.h"
#include "CFontUI.h"
#include "CSoundMgr.h"

CTarotSeller::CTarotSeller(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_ePreState(TAROT_END),
	m_eCurState(TAROT_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CTarotSeller::CTarotSeller(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_ePreState(TAROT_END),
	m_eCurState(TAROT_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}


CTarotSeller::CTarotSeller(const CTarotSeller& rhs)
	: CGameObject(rhs),
	m_ePreState(TAROT_END),
	m_eCurState(TAROT_IDLE),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CTarotSeller::~CTarotSeller()
{
}

HRESULT CTarotSeller::Ready_GameObject()
{
	m_eOBJID = OID_NPC;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	_vec3 vDialPos = m_vPos;
	vDialPos.y += 7.5f;

	Ready_Dialogue(vDialPos);

	return S_OK;
}

_int CTarotSeller::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_pFontUI->Update_GameObject(fTimeDelta);
	m_pSpeechBubble->Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		return iExit;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CTarotSeller::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTarotSeller::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CTarotSeller::OnCollision(CGameObject* pObject)
{
}

HRESULT CTarotSeller::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TarotSellerTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CTarotSeller::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = 7.f;
	m_fGroundY = -2.5f + fScale * 0.5f;
	m_iHp = 10;

	// Transform 세팅
	m_pTransformCom->Set_Pos(_float(rand() % 20), m_fGroundY, _float(rand() % 20) + 80.f);
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);


	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

HRESULT CTarotSeller::Ready_Dialogue(const _vec3& vDialoguePos)
{
	m_pFontUI = CFontUI::Create(m_pGraphicDev, m_pMessageChannel);

	if (m_pFontUI == nullptr) { return E_FAIL; }

	m_pFontUI->Set_Font(L"Font_Lapture30");
	m_pFontUI->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	//DT_CENTER | DT_VCENTER
	m_pFontUI->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pFontUI->Set_Scale(_vec2(600.f, 150.f));

	m_pFontUI->Set_WorldPos(vDialoguePos);
	m_pFontUI->Set_RenderOwnerName(L"클라우넥");

	m_pSpeechBubble = CSpeechBubble::Create(m_pGraphicDev, vDialoguePos, _vec2(600.f, 150.f));

	if (m_pSpeechBubble == nullptr) { return E_FAIL; }

	return S_OK;
}

void CTarotSeller::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Dialogue", m_pMessageChannel->Subscribe(L"CutScene.Dialogue", [this](const IMessageChannel::EVENT& Event)
		{
			auto TargetNameiter = Event.hmapData.find(L"TargetName");
			if (TargetNameiter == Event.hmapData.end()) { return; }
			if (any_cast<wstring>(TargetNameiter->second) != L"Tarot_Saller")
			{
				m_pSpeechBubble->UnActive();
				m_pFontUI->UnActive();
				m_eCurState = TAROT_IDLE;
				return;
			}

			auto Textiter = Event.hmapData.find(L"Text");
			if (Textiter == Event.hmapData.end()) { return; }

			m_pFontUI->Set_Text(any_cast<wstring>(Textiter->second));
			m_pFontUI->Set_OwnerName(any_cast<wstring>(TargetNameiter->second));
			m_pSpeechBubble->Active();
			m_pFontUI->Active();
			m_eCurState = TAROT_TALK;
			_tchar strSoundName[128] = L"";
			swprintf_s(strSoundName, L"GoatTalk%d.wav", Get_Rand_Int(1, 4));
			CSoundMgr::GetInstance()->Play(strSoundName, SOUND_DIALOGUE, 0.2f);
		}
	) });
}

void CTarotSeller::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case TAROT_IDLE:
	{
		m_fFrameEnd = 248.f;
	}
	break;

	case TAROT_TALK:
	{
		m_fFrameEnd = 80.f;
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CTarotSeller::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		switch (m_eCurState)
		{
		case TAROT_IDLE:
			m_eCurState = TAROT_TALK;
			break;

		case TAROT_TALK:
			m_eCurState = TAROT_IDLE;
			break;
		}
	}
}

void CTarotSeller::Set_Texture()
{
	//_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 16;
	_uint iV = iFrame / 16;

	m_matTex._11 = 0.0625f;	// 가로는 16칸 고정
	m_matTex._22 = 0.0625f;	// 세로는 16칸 고정(TarotSeller)

	switch (m_eCurState)
	{
	case TAROT_IDLE:
		break;

	case TAROT_TALK:
		break;
	}

	//if (bFilpX)
	//{
	//	m_matTex._11 *= -1.f;
	//	m_matTex._31 = _float(iU + 1) * 0.125f;	// 반전 O : 오른쪽에서 왼쪽으로 읽음
	//}
	//else
	//{
	m_matTex._31 = _float(iU) * 0.0625f;	// 반전 X : 왼쪽에서 오른쪽으로 읽음
	//}

	m_matTex._32 = _float(iV) * 0.0625f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(_uint(m_eCurState));
}

CTarotSeller* CTarotSeller::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos)
{
	CTarotSeller* pTarotSeller = new CTarotSeller(pGraphicDev, StageChannel);

	pTarotSeller->m_vPos = vPos;

	if (FAILED(pTarotSeller->Ready_GameObject()))
	{
		Safe_Release(pTarotSeller);
		MSG_BOX("pTarotSeller Create Failed");
		return nullptr;
	}

	pTarotSeller->m_pTransformCom->Set_Pos(vPos.x, pTarotSeller->m_fGroundY, vPos.z);

	return pTarotSeller;
}

void CTarotSeller::Free()
{
	Safe_Release(m_pFontUI);
	Safe_Release(m_pSpeechBubble);
	CGameObject::Free();
}
