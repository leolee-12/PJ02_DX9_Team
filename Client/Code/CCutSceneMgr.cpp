#include "pch.h"
#include "CCutSceneMgr.h"
#include "CDInputMgr.h"
#include "Trigger_Enum.h"
#include "CSoundMgr.h"
#include "CEffectMgr.h"
#include "CLetterBox.h"

CCutSceneMgr* CCutSceneMgr::m_pInstance = nullptr;

CCutSceneMgr::CCutSceneMgr()
	: m_pCurrentCutScene(nullptr), m_pMessageChannel(nullptr), m_bPlaying(false)
	, m_iCurrentStep(0), m_bDialogueEnd(true)
{
}

CCutSceneMgr::~CCutSceneMgr()
{
	Free();
}

void CCutSceneMgr::Register_CutScene(const CUTSCENE& tCutScene)
{
	m_hmapCutScenes[tCutScene.strName] = tCutScene;
}

_int CCutSceneMgr::Update_CutScene(const _float fTimeDelta)
{
	if (!m_bPlaying) { return NOEVENT; }
	if (!m_pCurrentCutScene) { return NOEVENT; }

	CUTSCENE_STEP& tStep = m_pCurrentCutScene->vecSteps[m_iCurrentStep];

	switch (tStep.eAdvanceType)
	{
	case ADV_DIALOGUE:
		//Key_Input_CutScene();
		break;

	case ADV_TIMED:
		m_fAccTime += fTimeDelta;
		if (m_fAccTime >= tStep.fDuration)
		{
			m_fAccTime = 0.f;
			Next_Step();
		}
		break;

	case ADV_EVENT:
		// Execute_Step에서 이벤트 구독해둠 → 이벤트 수신시 m_bEventReceived = true
		if (m_bEventReceived)
		{
			m_bEventReceived = false;
			Next_Step();
		}
		break;

	case ADV_IMMEDIATE:
		// 즉시 다음 스텝 (카메라 이동만 하고 바로 넘어가는 용도)
		Next_Step();
		break;
	}

	return NOEVENT;
}

void CCutSceneMgr::LateUpdate_CutScene(const _float fTimeDelta)
{
	if (!m_bPlaying) { return; }

	CUTSCENE_STEP& tStep = m_pCurrentCutScene->vecSteps[m_iCurrentStep];

	switch (tStep.eAdvanceType)
	{
	case ADV_DIALOGUE:
		Key_Input_CutScene();
		break;
	case ADV_TIMED:
		break;
	case ADV_EVENT:
		break;
	case ADV_IMMEDIATE:
		break;
	}
}

void CCutSceneMgr::Play_CutScene(const wstring& strName)
{
	unordered_map<wstring, CUTSCENE>::iterator iter = m_hmapCutScenes.find(strName);
	if (iter == m_hmapCutScenes.end()) { return; }

	m_pCurrentCutScene = &(iter->second);
	m_iCurrentStep = 0;
	m_bPlaying = true;

	CEffectMgr::GetInstance()->Get_LetterBox()->Play();

	Execute_Step(m_iCurrentStep);
}

void CCutSceneMgr::Clear_CutScene()
{
	if (m_bPlaying) { End_CutScene(); }

	Unsubscribe_Handles();
	Safe_Release(m_pMessageChannel);
	m_pCurrentCutScene = nullptr;
	m_hmapCutScenes.clear();
}

void CCutSceneMgr::Subscribe()
{
	m_hmapSubHandles.insert({ L"Trigger.Activate", m_pMessageChannel->Subscribe(L"Trigger.Activate", [this](const IMessageChannel::EVENT& Event)
		{
			auto iter = Event.hmapData.find(L"Trigger_TID");
			if (iter == Event.hmapData.end()) { return; }

			if (any_cast<Trigger::TRIGGERID>(iter->second) == Trigger::TI_STAGING)
			{
				auto stagingiter = Event.hmapData.find(L"Trigger_Name");
				if (stagingiter == Event.hmapData.end()) { return; }

				Play_CutScene(any_cast<wstring>(stagingiter->second));
			}
		}
	) });

	m_hmapSubHandles.insert({ L"Staging.Start", m_pMessageChannel->Subscribe(L"Staging.Start", [this](const IMessageChannel::EVENT& Event)
		{
			auto stagingiter = Event.hmapData.find(L"StagingName");
			if (stagingiter == Event.hmapData.end()) { return; }

			Play_CutScene(any_cast<wstring>(stagingiter->second));
		}
	) });

	m_hmapSubHandles.insert({ L"Dialogue.End", m_pMessageChannel->Subscribe(L"Dialogue.End", [this](const IMessageChannel::EVENT& Event)
		{
			m_bDialogueEnd = true;
		}
	) });

	m_hmapSubHandles.insert({ L"Choice.Selected", m_pMessageChannel->Subscribe(L"Choice.Selected", [this](const IMessageChannel::EVENT& Event)
		{
			m_bChoiceSelected = true;
			Next_Step();
		}
	) });
}

void CCutSceneMgr::Key_Input_CutScene()
{
	if (!m_pCurrentCutScene) { return; }

	if (CDInputMgr::GetInstance()->Key_Down(DIK_RETURN))
	{
		if (m_bChoiceShow) { return; }
		if (m_bDialogueEnd)
		{
			CUTSCENE_STEP& tStep = m_pCurrentCutScene->vecSteps[m_iCurrentStep];

			if (!tStep.vecChoiceTexts.empty())
			{
				IMessageChannel::EVENT tChoiceEvent;
				tChoiceEvent.strType = L"CutScene.ShowChoice";
				tChoiceEvent.hmapData[L"SceneName"] = m_pCurrentCutScene->strName;
				tChoiceEvent.hmapData[L"Choices"] = tStep.vecChoiceTexts;
				m_pMessageChannel->Publish(tChoiceEvent);

				m_bChoiceShow = true;
				return;
			}

			CSoundMgr::GetInstance()->StopSound(SOUND_DIALOGUE);
			Next_Step();
		}
		else
		{
			CUTSCENE_STEP& tStep = m_pCurrentCutScene->vecSteps[m_iCurrentStep];

			IMessageChannel::EVENT tDialogueEvent;
			tDialogueEvent.strType = L"CutScene.Skip";
			tDialogueEvent.hmapData[L"Text"] = tStep.strFont;
			tDialogueEvent.hmapData[L"TargetName"] = tStep.strTargetName;
			m_pMessageChannel->Publish(tDialogueEvent);
		}
	}
}

void CCutSceneMgr::Next_Step()
{
	m_bChoiceSelected = false;
	m_bChoiceShow = false;

	// 이전 스텝이 ADV_EVENT였으면 구독 해제
	if (m_bHasWaitHandle)
	{
		m_pMessageChannel->Unsubscribe(m_hWaitEventHandle);
		m_bHasWaitHandle = false;
	}

	++m_iCurrentStep;

	if (m_iCurrentStep >= _uint(m_pCurrentCutScene->vecSteps.size()))
	{
		End_CutScene();
		return;
	}
	
	Execute_Step(m_iCurrentStep);
}

void CCutSceneMgr::Execute_Step(_uint iStep)
{
	CUTSCENE_STEP& tStep = m_pCurrentCutScene->vecSteps[iStep];

	IMessageChannel::EVENT tCamEvent;
	tCamEvent.strType = L"CutScene.CameraTarget";
	tCamEvent.hmapData[L"TargetPos"] = tStep.vTargetPos;
	tCamEvent.hmapData[L"Lerp"] = tStep.fLerp;
	tCamEvent.hmapData[L"Zoom"] = tStep.fZoom;
	m_pMessageChannel->Publish(tCamEvent);



	IMessageChannel::EVENT tDialogueEvent;
	tDialogueEvent.strType = L"CutScene.Dialogue";
	if (tStep.eAdvanceType == ADV_DIALOGUE)
	{
		tDialogueEvent.hmapData[L"Text"] = tStep.strFont;
		tDialogueEvent.hmapData[L"TargetName"] = tStep.strTargetName;
		m_bDialogueEnd = tStep.strFont.empty();
	}
	else
	{
		// 시네마틱: 빈 타겟을 보내서 다이얼로그를 전부 닫게하고,
		// 타겟네임을 시네마 타겟 네임으로 보내고,
		// 다이얼로그의 텍스트에 해당하는 내용을 특정 행동을 하는 키로 보냄.
		tDialogueEvent.hmapData[L"TargetName"] = wstring(L"");
		tDialogueEvent.hmapData[L"CinemaTargetName"] = tStep.strTargetName;
		tDialogueEvent.hmapData[L"Dothis"] = tStep.strFont;
	}
	m_pMessageChannel->Publish(tDialogueEvent);

	switch (tStep.eAdvanceType)
	{
	case ADV_TIMED:
		m_fAccTime = 0.f;
		break;
	case ADV_EVENT:
		m_bEventReceived = false;
		m_hWaitEventHandle = m_pMessageChannel->Subscribe(
			tStep.strWaitEventType,
			[this](const IMessageChannel::EVENT& Event)
			{
				m_bEventReceived = true;
			}
		);
		m_bHasWaitHandle = true;
		break;
	case ADV_IMMEDIATE:
		break;
	}


	if (tStep.strFont.empty()) { return; }

	m_bDialogueEnd = false;
}

void CCutSceneMgr::End_CutScene()
{
	m_bPlaying = false;
	wstring CurSceneName = m_pCurrentCutScene->strName;

	m_pCurrentCutScene = nullptr;

	IMessageChannel::EVENT tEndEvent;
	tEndEvent.strType = L"CutScene.End";
	tEndEvent.hmapData[L"SceneName"] = CurSceneName;
	m_pMessageChannel->Publish(tEndEvent);

	CEffectMgr::GetInstance()->Get_LetterBox()->Exit();
}

void CCutSceneMgr::Unsubscribe_Handles()
{
	for (auto iter = m_hmapSubHandles.begin();
		iter != m_hmapSubHandles.end();)
	{
		m_pMessageChannel->Unsubscribe(iter->second);
		iter = m_hmapSubHandles.erase(iter);
	}
}

void CCutSceneMgr::Free()
{
	Clear_CutScene();
}
