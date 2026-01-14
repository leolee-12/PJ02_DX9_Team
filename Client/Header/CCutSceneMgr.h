#pragma once
#include "CBase.h"
#include "CGameObject.h"
#include "Engine_Define.h"

namespace Engine
{
	class CTransform;
}

typedef struct tagCutStep
{
	_vec3 vTargetPos;
	_float fZoom;
	wstring strTargetName;
	wstring strFont;

} CUTSCENE_STEP;

typedef struct tagCutScene
{
	//wstring strName;
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
	void		Register_CutScene(const wstring& strName, const CUTSCENE& tCutScene);

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

private:
	static CCutSceneMgr* m_pInstance;

private:
	virtual void		Free();
};