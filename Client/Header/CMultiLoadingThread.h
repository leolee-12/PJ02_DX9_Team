#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include <concurrent_queue.h>

const _uint WORKER_COUNT = 14;

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
	
	void        Loading_for_Tutorial();
	void		Loading_for_Dungeon();
	void        Loading_for_KnuckleBone();

	void		NonTex_for_Dungeon();
	void		NonTex_for_Tutorial();


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

	HANDLE				m_hThread[WORKER_COUNT];
	LOADINGID			m_eLoading;


	LPDIRECT3DDEVICE9					m_pGraphicDev;


};

