#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include <concurrent_queue.h>

const _uint WORKER_COUNT = []() {
	unsigned int cores = std::thread::hardware_concurrency();
	return (cores > 1) ? cores - 1 : 1;
	}();



class CMultiLoadingThread : public CBase
{
public:
	using TaskFunc = void (CMultiLoadingThread::*)();

private:
	explicit CMultiLoadingThread(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMultiLoadingThread();

public:
	LOADINGID				Get_Loading() { return m_eLoading; }
	_uint					Get_TotalCount() { return m_iTotalProtoCount; }
	_uint					Get_CompletedCount() { return m_iCompletedCount; }
	//_uint					Get_LoadCount() { return m_iLoadCount; }

	//Concurrency::concurrent_queue<TEXLR>& Get_ReadyQueue() { return m_TextureReadyqueue; }
	//Concurrency::concurrent_queue<pair<TEXSETINFO, vector<TEXSETLR>>>& Get_TexSetReadyqueue() { return m_TexSetReadyqueue; }
public:
	HRESULT		Ready_Loading(LOADINGID eID);
	HRESULT		Ready_Loadingqueue();

	void        Loading_for_Logo();
	void		Loading_for_Stage();
	void        Loading_for_Boss();
	void        Loading_for_Test();
	void        Loading_for_KnuckleBone();
	void		Loading_for_Dungeon();
	void        Loading_for_Tutorial();
	void        Loading_for_TheGateway();
	void        Loading_for_Village();
	void		Loading_for_RealDungeon();
	void        Loading_for_AmdusiasRoom();
	void        Loading_for_LeshyRoom();
	

	void        NonTex_for_Logo();
	void		NonTex_for_Stage();
	void        NonTex_for_Boss();
	void        NonTex_for_Test();
	void        NonTex_for_KnuckleBone();
	void		NonTex_for_Dungeon();
	void        NonTex_for_Tutorial();
	void        NonTex_for_TheGateway();
	void        NonTex_for_Village();
	void		NonTex_for_RealDungeon();
	void        NonTex_for_AmdusiasRoom();
	void        NonTex_for_LeshyRoom();


	void		Load_Object_Texture(const std::set<std::wstring>& textureSet);
public:
	static unsigned int CALLBACK Thread_Main(void* pArg);

public:
	static CMultiLoadingThread* Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID);

private:
	virtual void Free();

private:
	Concurrency::concurrent_queue<TEXLR> m_TexturLoadingqueue;
	//Concurrency::concurrent_queue<TEXLR> m_TextureReadyqueue;

	Concurrency::concurrent_queue<pair<TEXSETINFO, vector<TEXSETLR>>> m_TexSetLoadingqueue;
	//Concurrency::concurrent_queue<pair<TEXSETINFO, vector<TEXSETLR>>> m_TexSetReadyqueue;

	Concurrency::concurrent_queue<TaskFunc> m_NonTexReadyQueue;


	//atomic<_uint> m_iLoadCount;
	atomic<_uint> m_iCompletedCount;
	_uint m_iTotalProtoCount;

	vector<HANDLE>		m_vecThread;
	LOADINGID			m_eLoading;


	LPDIRECT3DDEVICE9					m_pGraphicDev;




};

