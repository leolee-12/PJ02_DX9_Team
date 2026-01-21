#include "pch.h"
#include "CRatau.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"
#include "CCollisionMgr.h"
#include "CN1_AI.h"
#include "CSpeechBubble.h"
#include "CFontUI.h"
#include "CSoundMgr.h"

CRatau::CRatau(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev),
	m_ePreState(RATAU_END),
	m_eCurState(RATAU_ENTER),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CRatau::CRatau(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
	: CGameObject(pGraphicDev, StageChannel),
	m_ePreState(RATAU_END),
	m_eCurState(RATAU_ENTER),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}


CRatau::CRatau(const CRatau& rhs)
	: CGameObject(rhs),
	m_ePreState(RATAU_END),
	m_eCurState(RATAU_ENTER),
	m_fFrame(0.f),
	m_fFrameEnd(0.f),
	m_fFrameSpeed(0.f)
{
}

CRatau::~CRatau()
{
}

HRESULT CRatau::Ready_GameObject()
{
	m_eOBJID = OID_NPC;

	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Variable();
	Ready_Event();

	_vec3 vDialPos = m_vPos;
	vDialPos.y += 5.f;

	Ready_Dialogue(vDialPos);

	return S_OK;
}

_int CRatau::Update_GameObject(const _float& fTimeDelta)
{
	Move_Frame(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_pFontUI->Update_GameObject(fTimeDelta);
	m_pSpeechBubble->Update_GameObject(fTimeDelta);

	if (iExit == DEAD)
	{
		return iExit;
	}

	if (!m_bWait) {
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	}

	return iExit;
}

void CRatau::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Check_Frame();

	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CRatau::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	Set_Texture();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CRatau::OnCollision(CGameObject* pObject)
{
}

HRESULT CRatau::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RatauTexture"));

	NULL_CHECK_RETURN(pComponent, E_FAIL)

		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CRatau::Ready_Variable()
{
	// 게임로직 변수 세팅
	_float fScale = 10.f;
	m_fGroundY = -2.5f + fScale * 0.5f - 3.f;
	m_iHp = 10;

	// Transform 세팅
	m_pTransformCom->Set_Pos(125.f, m_fGroundY, 15.f);
	m_pTransformCom->Set_Scale(fScale, fScale, fScale);

	// Anim 관련 세팅
	m_fFrameSpeed = 24.f;
	D3DXMatrixIdentity(&m_matTex);
}

void CRatau::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Dialogue", m_pMessageChannel->Subscribe(L"CutScene.Dialogue", [this](const IMessageChannel::EVENT& Event)
		{
			auto TargetNameiter = Event.hmapData.find(L"TargetName");
			if (TargetNameiter == Event.hmapData.end()) { return; }
			if (any_cast<wstring>(TargetNameiter->second) != L"Ratau")
			{
				m_pSpeechBubble->UnActive();
				m_pFontUI->UnActive();
				m_eCurState = RATAU_IDLE;

				auto CinemaTargetNameiter = Event.hmapData.find(L"CinemaTargetName");
				if (CinemaTargetNameiter == Event.hmapData.end()) { return; }
				auto Dothisiter = Event.hmapData.find(L"Dothis");
				if (Dothisiter == Event.hmapData.end()) { return; }
				if (any_cast<wstring>(CinemaTargetNameiter->second) == L"Ratau")
				{
					wstring strDothis = any_cast<wstring>(Dothisiter->second);
					if (strDothis == L"Ratau_Intro") {
						m_eCurState = RATAU_ENTER;
						m_bWait = false;
						return;
					}
					if (strDothis == L"Ratau_Outro") {
						m_eCurState = RATAU_EXIT;
						return;
					}
				}

				return;
			}

			auto Textiter = Event.hmapData.find(L"Text");
			if (Textiter == Event.hmapData.end()) { return; }

			m_pFontUI->Set_Text(any_cast<wstring>(Textiter->second));
			m_pFontUI->Set_OwnerName(any_cast<wstring>(TargetNameiter->second));
			m_pSpeechBubble->Active();
			m_pFontUI->Active();
			m_eCurState = RATAU_TALK;
			_tchar strSoundName[128] = L"";
			swprintf_s(strSoundName, L"Ratau_Dial%d.wav", Get_Rand_Int(1, 15));
			CSoundMgr::GetInstance()->Play(strSoundName, SOUND_DIALOUGE, 0.1f);
		}
	) });

	m_hmapSubHandles.insert({ L"CutScene.End", m_pMessageChannel->Subscribe(L"CutScene.End", [this](const IMessageChannel::EVENT& Event) {
		wstring SceneName = any_cast<wstring>(Event.hmapData.find(L"SceneName")->second);
	if (SceneName == L"Ratau_01")
	{
		m_iHp = 0;
	}
	if (SceneName == L"Ratau_00")
	{
		m_iHp = 0;
	}
	}) });
}

HRESULT CRatau::Ready_Dialogue(const _vec3& vDialoguePos)
{
	m_pFontUI = CFontUI::Create(m_pGraphicDev, m_pMessageChannel);

	if (m_pFontUI == nullptr) { return E_FAIL; }

	m_pFontUI->Set_Font(L"Font_Lapture30");
	m_pFontUI->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	//DT_CENTER | DT_VCENTER
	m_pFontUI->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pFontUI->Set_Scale(_vec2(600.f, 150.f));

	m_pFontUI->Set_WorldPos(vDialoguePos);
	m_pFontUI->Set_RenderOwnerName(L"라타우");

	m_pSpeechBubble = CSpeechBubble::Create(m_pGraphicDev, vDialoguePos, _vec2(600.f, 150.f));

	if (m_pSpeechBubble == nullptr) { return E_FAIL; }

	return S_OK;
}

void CRatau::Check_Frame()
{
	if (m_ePreState == m_eCurState)
		return;

	m_fFrame = 0.f;

	switch (m_eCurState)
	{
	case RATAU_IDLE:
	{
		m_fFrameEnd = 64.f;
	}
	break;

	case RATAU_TALK:
	{
		m_fFrameEnd = 64.f;
	}
	break;

	case RATAU_ENTER:
	{
		m_fFrameEnd = 33.f;
	}
	break;

	case RATAU_EXIT:
	{
		m_fFrameEnd = 56.f;
	}
	break;
	}

	m_ePreState = m_eCurState;
}

void CRatau::Move_Frame(const _float& fTimeDelta)
{
	if (m_bWait) { return; }

	m_fFrame += m_fFrameSpeed * fTimeDelta;

	if (m_fFrame >= m_fFrameEnd)
	{
		m_fFrame = 0.f;

		IMessageChannel::EVENT RatauEvent;

		switch (m_eCurState)
		{
		case RATAU_ENTER:
			RatauEvent.strType = L"Ratau.Done";
			m_pMessageChannel->Publish(RatauEvent);
			m_eCurState = RATAU_IDLE;
			break;

		case RATAU_IDLE:
			break;

		case RATAU_TALK:
			break;

		case RATAU_EXIT:
			RatauEvent.strType = L"Ratau.Done";
			m_pMessageChannel->Publish(RatauEvent);
			m_bWait = true;
			break;
		}
	}
}

void CRatau::Set_Texture()
{
	//_bool bFilpX = vDir.x > 0.f ? true : false;	// 반전 여부
	_uint iFrame = _uint(m_fFrame);					// 현재 프레임

	D3DXMatrixIdentity(&m_matTex);
	_uint iU = iFrame % 8;
	_uint iV = iFrame / 8;

	m_matTex._11 = 0.125f;	// 가로는 8칸 고정
	m_matTex._22 = 0.125f;	// 세로는 8칸 고정(Ratau)

	switch (m_eCurState)
	{
	case RATAU_IDLE:
		break;

	case RATAU_TALK:
		break;

	case RATAU_ENTER:
		break;

	case RATAU_EXIT:
		break;
	}

	//if (bFilpX)
	//{
	//	m_matTex._11 *= -1.f;
	//	m_matTex._31 = _float(iU + 1) * 0.125f;	// 반전 O : 오른쪽에서 왼쪽으로 읽음
	//}
	//else
	//{
		m_matTex._31 = _float(iU) * 0.125f;	// 반전 X : 왼쪽에서 오른쪽으로 읽음
	//}

	m_matTex._32 = _float(iV) * 0.125f;

	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_matTex);

	m_pTextureCom->Set_Texture(_uint(m_eCurState));
}

CRatau* CRatau::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos)
{
	CRatau* pRatau = new CRatau(pGraphicDev, StageChannel);

	pRatau->m_vPos = vPos;

	if (FAILED(pRatau->Ready_GameObject()))
	{
		Safe_Release(pRatau);
		MSG_BOX("pRatau Create Failed");
		return nullptr;
	}

	pRatau->m_pTransformCom->Set_Pos(vPos.x, pRatau->m_fGroundY, vPos.z);

	return pRatau;
}

void CRatau::Free()
{
	Safe_Release(m_pFontUI);
	Safe_Release(m_pSpeechBubble);
	CGameObject::Free();
}
