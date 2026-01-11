#pragma once
#include "CBase.h"
#include "Engine_Define.h"

class CLoadingThread :    public CBase
{
public:
	explicit CLoadingThread(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoadingThread();

public:
	CRITICAL_SECTION*		Get_Crt() { return &m_Crt; }
	LOADINGID				Get_Loading() { return m_eLoading; }
	_bool					Get_Finish() { return m_bFinish; }
	_float					Get_Clamp_Percent() { return m_fPercent / 100.f; }

	const _tchar* Get_String() { return m_szLoading; }

public:
	HRESULT		Ready_Loading(LOADINGID eID);
	_uint		Loading_ForTest();
	_uint		Loading_ForStage();

	/* Loading For Map */
	_uint		Loading_MapCommon();

	_uint		Loading_ForTutorial();
	_uint		Loading_ForTheGateway();
	_uint		Loading_ForDungeon();
	_uint		Loading_ForVillage();
	_uint		Loading_ForRealDungeon();
	_uint		Loading_ForAmdusiasRoom();
	_uint		Loading_ForLeshyRoom();

	/* Loading for minigame */
	_uint		Loading_ForKnuckleBone();

	void		Load_Object_Texture(const std::set<std::wstring>& textureSet);

public:
	static unsigned int CALLBACK Thread_Main(void* pArg);


private:

	LPDIRECT3DDEVICE9	m_pGraphicDev;
	HANDLE				m_hThread;
	LOADINGID			m_eLoading;

	CRITICAL_SECTION	m_Crt;
	_float				m_fPercent;
	_bool				m_bFinish;

	_tchar				m_szLoading[128];

public:
	static CLoadingThread* Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID);

private:
	virtual void		Free();

};

