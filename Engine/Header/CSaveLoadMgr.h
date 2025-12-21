#pragma once
#include "CBase.h"
#include "Engine_Define.h"

/* -------------------------------------------
   CSaveLoadManager

   맵툴에서 작성한 파일을 저장하고 부르기 위한 매니저 클래스
-------------------------------------------- */
BEGIN(Engine)

class ENGINE_DLL CSaveLoadMgr : public CBase
{
	DECLARE_SINGLETON(CSaveLoadMgr)

private:
	explicit CSaveLoadMgr();
	virtual ~CSaveLoadMgr();

public:
	void Ready_SaveLoad();

	void Save(string fileName);
	void Load(string fileName);

private:
	virtual void Free();

	vector<pair<string, Json::Value>> MapSet;
};

END

