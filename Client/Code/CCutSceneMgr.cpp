#include "pch.h"
#include "CCutSceneMgr.h"
#include "CDInputMgr.h"
#include "Trigger_Enum.h"

CCutSceneMgr* CCutSceneMgr::m_pInstance = nullptr;

CCutSceneMgr::CCutSceneMgr()
	: m_pCurrentCutScene(nullptr), m_pMessageChannel(nullptr), m_bPlaying(false)
	, m_iCurrentStep(0)
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

	return NOEVENT;
}

void CCutSceneMgr::LateUpdate_CutScene(const _float fTimeDelta)
{
	if (!m_bPlaying) { return; }

	Key_Input_CutScene();
}

void CCutSceneMgr::Play_CutScene(const wstring& strName)
{
	unordered_map<wstring, CUTSCENE>::iterator iter = m_hmapCutScenes.find(strName);
	if (iter == m_hmapCutScenes.end()) { return; }

	m_pCurrentCutScene = &(iter->second);
	m_iCurrentStep = 0;
	m_bPlaying = true;

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
	m_hmapSubHandles.insert({ L"Trigger.Activate", m_pMessageChannel->Subscribe(L"Trigger.Activate", [this](const IMessageChannel::EVENT& Event) {
		auto iter = Event.hmapData.find(L"Trigger_TID");
		if (iter == Event.hmapData.end()) { return; }

		if (any_cast<Trigger::TRIGGERID>(iter->second) == Trigger::TI_STAGING)
		{
			auto stagingiter = Event.hmapData.find(L"Trigger_Name");
			if (stagingiter == Event.hmapData.end()) { return; }

			Play_CutScene(any_cast<wstring>(stagingiter->second));
		}
		
	}) });
}

void CCutSceneMgr::Key_Input_CutScene()
{
	if (CDInputMgr::GetInstance()->Key_Down(DIK_RETURN))
	{
		Next_Step();
	}
}

void CCutSceneMgr::Next_Step()
{
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
	tCamEvent.hmapData[L"Zoom"] = tStep.fZoom;
	m_pMessageChannel->Publish(tCamEvent);

	IMessageChannel::EVENT tDialogueEvent;
	tDialogueEvent.strType = L"CutScene.Dialogue";
	tDialogueEvent.hmapData[L"Text"] = tStep.strFont;
	tDialogueEvent.hmapData[L"TargetName"] = tStep.strTargetName;
	m_pMessageChannel->Publish(tDialogueEvent);

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