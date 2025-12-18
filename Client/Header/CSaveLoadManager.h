#pragma once
#include "Engine_Define.h"
#include "../Client/Header/ImGui_Define.h"

/* -------------------------------------------
   CSaveLoadManager

   맵툴에서 작성한 파일을 저장하고 부르기 위한 매니저 클래스
-------------------------------------------- */

class CSaveLoadManager
{
private:
	CSaveLoadManager();
	~CSaveLoadManager();

public:
	void SetUp();

public:
	CSaveLoadManager* GetInstnace()
	{
		if (Instance == nullptr)
		{
			Instance = new CSaveLoadManager();
		}
		return Instance;
	}

	void DestroyInstance()
	{
		if (Instance != nullptr)
		{
			delete Instance;
			Instance = nullptr;
		}
	}

private:
	CSaveLoadManager* Instance;

	vector<pair
};

