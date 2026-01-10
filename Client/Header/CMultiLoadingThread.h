#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include <concurrent_queue.h>

const _int WORKER_COUNT = 4;

class CMultiLoadingThread : public CBase
{
public:



private:
	explicit CMultiLoadingThread();
	virtual ~CMultiLoadingThread();

public:
	LOADINGID				Get_Loading() { return m_eLoading; }
	_bool					Get_Finish() { return m_bFinish; }
	_uint					Get_TotalCount() { return m_iTotalProtoCount; }

	Concurrency::concurrent_queue<TEXLR>& Get_ReadyQueue() { return m_ProtoReadyqueue; }
	Concurrency::concurrent_queue<pair<TEXSETINFO, vector<TEXSETLR>>>& Get_TexSetReadyqueue() { return m_TexSetReadyqueue; }
public:
	HRESULT		Ready_Loading(LOADINGID eID);
	HRESULT		Ready_Loadingqueue();
	
	void		Loading_for_Dungeon();
	void        Loading_for_KnuckleBone();

public:
	static unsigned int CALLBACK Thread_Main(void* pArg);

public:
	static CMultiLoadingThread* Create(LOADINGID eID);

private:
	virtual void Free();

private:
	Concurrency::concurrent_queue<TEXLR> m_ProtoLoadingqueue;
	Concurrency::concurrent_queue<TEXLR> m_ProtoReadyqueue;

	Concurrency::concurrent_queue<pair<TEXSETINFO, vector<TEXSETLR>>> m_TexSetLoadingqueue;
	Concurrency::concurrent_queue<pair<TEXSETINFO, vector<TEXSETLR>>> m_TexSetReadyqueue;

	_uint m_iTotalProtoCount;

	HANDLE				m_hThread[WORKER_COUNT];
	LOADINGID			m_eLoading;

	_bool				m_bFinish;


};

