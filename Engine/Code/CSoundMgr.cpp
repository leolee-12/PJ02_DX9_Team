#include "CSoundMgr.h"
#include "fmod.hpp"
#include <io.h>

CSoundMgr* CSoundMgr::m_pInstance = nullptr;

CSoundMgr::CSoundMgr()
	: m_pSystem(nullptr)
{
	ZeroMemory(m_pChannelArr, sizeof(m_pChannelArr));
}

CSoundMgr::~CSoundMgr()
{
	Free();
}

void CSoundMgr::Ready_SoundMgr()
{
	FMOD_RESULT result;

	// FMOD 시스템 생성
	result = FMOD::System_Create(&m_pSystem);
	if (result != FMOD_OK)
	{
		// 에러 처리
		return;
	}

	// FMOD 시스템 초기화
	// 매개변수: (최대 채널 수, 초기화 플래그, 추가 드라이버 데이터)
	result = m_pSystem->init(MAXCHANNEL, FMOD_INIT_NORMAL, nullptr);
	if (result != FMOD_OK)
	{
		// 에러 처리
		return;
	}

	// 사운드 파일 로드
	LoadSoundFile();
}

void CSoundMgr::Free()
{
	// 모든 사운드 정지
	StopAll();

	// 사운드 리소스 해제
	for (auto& pair : m_mapSound)
	{
		if (pair.second)
		{
			pair.second->release();
			pair.second = nullptr;
		}
	}
	m_mapSound.clear();

	// FMOD 시스템 종료 및 해제
	if (m_pSystem)
	{
		m_pSystem->close();
		m_pSystem->release();
		m_pSystem = nullptr;
	}
}

void CSoundMgr::Update()
{
	// FMOD 시스템 업데이트 (매 프레임 호출 필요)
	if (m_pSystem)
	{
		m_pSystem->update();
	}
}

void CSoundMgr::Play(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	auto iter = m_mapSound.find(pSoundKey);

	if (iter == m_mapSound.end())
	{
		OutputDebugString(L"[사운드 매니저] 사운드를 찾을 수 없음: ");
		OutputDebugString(pSoundKey);
		OutputDebugString(L"\n");
		return;
	}

	OutputDebugString(L"[사운드 매니저] 재생 시도: ");
	OutputDebugString(pSoundKey);
	OutputDebugString(L"\n");

	bool bPlaying = false;
	if (m_pChannelArr[eID])
	{
		m_pChannelArr[eID]->isPlaying(&bPlaying);
	}

	// 해당 채널에서 재생 중이면 정지
	if (bPlaying)
	{
		m_pChannelArr[eID]->stop();
	}

	// 사운드 재생
	FMOD_RESULT result = m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[eID]);

	if (result == FMOD_OK)
	{
		OutputDebugString(L"[사운드 매니저] 재생 성공!\n");
		// 볼륨 설정 (0.0 ~ 1.0)
		m_pChannelArr[eID]->setVolume(fVolume);
	}
	else
	{
		OutputDebugString(L"[사운드 매니저] 재생 실패!\n");
	}


}

void CSoundMgr::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	auto iter = m_mapSound.find(pSoundKey);

	if (iter == m_mapSound.end())
		return;

	// BGM 채널에서 재생
	m_pSystem->playSound(iter->second, nullptr, false, &m_pChannelArr[SOUND_BGM]);

	// 볼륨 설정
	m_pChannelArr[SOUND_BGM]->setVolume(fVolume);

	// 무한 반복 설정
	m_pChannelArr[SOUND_BGM]->setMode(FMOD_LOOP_NORMAL);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	if (m_pChannelArr[eID])
	{
		m_pChannelArr[eID]->stop();
	}
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
	{
		if (m_pChannelArr[i])
		{
			m_pChannelArr[i]->stop();
		}
	}
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	if (m_pChannelArr[eID])
	{
		m_pChannelArr[eID]->setVolume(fVolume);
	}
}

void CSoundMgr::LoadSoundFile()
{
	LoadSoundFileRecursive("../Bin/Resource/Sound/");
}

void CSoundMgr::LoadSoundFileRecursive(const char* pPath)
{
	_finddata_t fd;
	char szSearchPath[256] = "";
	sprintf_s(szSearchPath, "%s*.*", pPath);

	intptr_t handle = _findfirst(szSearchPath, &fd);

	if (handle == -1)
	{
		// 경로를 찾지 못함!
		OutputDebugStringA("사운드 폴더를 찾을 수 없습니다: ");
		OutputDebugStringA(pPath);
		OutputDebugStringA("\n");
		return;
	}

	int iResult = 0;

	while (iResult != -1)
	{
		// "."과 ".." 제외
		if (!strcmp(fd.name, ".") || !strcmp(fd.name, ".."))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		// 전체 경로 생성
		char szFullPath[256] = "";
		sprintf_s(szFullPath, "%s%s", pPath, fd.name);

		// 디렉터리인 경우 재귀 호출
		if (fd.attrib & _A_SUBDIR)
		{
			char szSubPath[256] = "";
			sprintf_s(szSubPath, "%s/", szFullPath);
			LoadSoundFileRecursive(szSubPath);
		}
		else // 파일인 경우
		{
			FMOD::Sound* pSound = nullptr;
			FMOD_RESULT result = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, nullptr, &pSound);

			if (result == FMOD_OK)
			{
				// 파일명만 키로 사용 (경로 제외)
				wstring strSoundKey;

#ifdef _UNICODE
				int iLength = MultiByteToWideChar(CP_ACP, 0, fd.name, -1, nullptr, 0);
				strSoundKey.resize(iLength - 1);
				MultiByteToWideChar(CP_ACP, 0, fd.name, -1, &strSoundKey[0], iLength);
#else
				strSoundKey = fd.name;
#endif

				m_mapSound.emplace(strSoundKey, pSound);

				if (result == FMOD_OK)
				{
					// 로드 성공 로그
					OutputDebugStringA("사운드 로드 성공: ");
					OutputDebugStringA(fd.name);
					OutputDebugStringA("\n");
				}
			}
		}

		iResult = _findnext(handle, &fd);
	}

	_findclose(handle);
}