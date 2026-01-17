#pragma once
#include "CBase.h"
#include "CGameObject.h"
#include "Engine_Define.h"

namespace Engine
{
	class CTransform;
}

enum CUTSTEP_ADVANCE
{
	ADV_DIALOGUE,
	ADV_TIMED,
	ADV_EVENT,
	ADV_IMMEDIATE,
	ADV_END
};

typedef struct tagCutStep
{
	_vec3 vTargetPos;
	_float fZoom;
	_float fLerp;
	wstring strTargetName;
	wstring strFont;

	CUTSTEP_ADVANCE eAdvanceType = ADV_DIALOGUE;
	_float fDuration = 0.f;			// ADV_TIMED용
	wstring strWaitEventType;		// ADV_EVENT용 (구독할이벤트명)

	vector<wstring> vecChoiceTexts;

} CUTSCENE_STEP;

typedef struct tagCutScene
{
	wstring strName;
	vector<CUTSCENE_STEP> vecSteps;

} CUTSCENE ;

class CCutSceneMgr : public CBase
{
private:
	explicit CCutSceneMgr();
	explicit CCutSceneMgr(const CCutSceneMgr& rhs) = delete;
	CCutSceneMgr operator = (const CCutSceneMgr&) = delete;
	virtual ~CCutSceneMgr();

public:
	void		Register_CutScene(const CUTSCENE& tCutScene);

	HRESULT		Ready_CutsceneMgr(IMessageChannel* pMessageChannel)
	{
		if (m_pMessageChannel) { Safe_Release(m_pMessageChannel); }
		m_pMessageChannel = pMessageChannel;
		if (m_pMessageChannel == nullptr)
		{
			MSG_BOX("컷씬매니저 매세지채널 등록 실패");
			return E_FAIL;
		}
		m_pMessageChannel->AddRef();
		Subscribe();

		return S_OK;
	}

	_bool		Get_Playing() { return m_bPlaying; }

public:
	_int		Update_CutScene(const _float fTimeDelta);
	void		LateUpdate_CutScene(const _float fTimeDelta);
	void		Play_CutScene(const wstring& strName);
	void		Clear_CutScene();




public:
	static CCutSceneMgr* GetInstance()
	{
		if (m_pInstance == nullptr) {
			m_pInstance = new CCutSceneMgr;
		}

		return m_pInstance;
	}
	static void DestroyInstance()
	{
		if (nullptr != m_pInstance) {

			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	void		Subscribe();
	void		Key_Input_CutScene();

	void		Next_Step();
	void		Execute_Step(_uint iStep);
	void		End_CutScene();

	void		Unsubscribe_Handles();



private:
	unordered_map<wstring, CUTSCENE> m_hmapCutScenes;
	IMessageChannel* m_pMessageChannel;

	unordered_map<wstring, IMessageChannel::SUBHANDLE> m_hmapSubHandles;

	CUTSCENE* m_pCurrentCutScene;
	_uint m_iCurrentStep;
	_bool m_bPlaying;

	_bool m_bDialogueEnd;

	_float m_fAccTime = 0.f;                              // ADV_TIMED용 누적 시간
	_bool m_bEventReceived = false;                       // ADV_EVENT용 플래그
	IMessageChannel::SUBHANDLE m_hWaitEventHandle;		  // ADV_EVENT용 구독 핸들
	_bool m_bHasWaitHandle = false;						  // 구독핸들 값확인용 플래그

	// 선택지 관련 변수
	_bool m_bChoiceShow = false;			// 선택지 ui 표시
	_bool m_bChoiceSelected = false;		// 선택 완료됨

private:
	static CCutSceneMgr* m_pInstance;

private:
	virtual void		Free();
};
