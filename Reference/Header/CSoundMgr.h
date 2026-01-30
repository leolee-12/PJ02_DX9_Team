#pragma once
#include "Engine_Define.h"

// FMOD 전방 선언
namespace FMOD
{
	class Sound;
	class Channel;
	class System;
}

BEGIN(Engine)

class ENGINE_DLL CSoundMgr
{
public:
	static CSoundMgr* GetInstance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CSoundMgr;
		return m_pInstance;
	}
	static void DestroyInstance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	CSoundMgr();
	~CSoundMgr();
public:
	void Ready_SoundMgr();
	void Free();
	void Update(); // FMOD 시스템 업데이트 (매 프레임 호출 필요)
public:
	void Play(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlaySound3D(const TCHAR* pSoundKey, CHANNELID eID, float fVolume, const _vec3& vSoundPos);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

	void Set_ListenerPos(const _vec3& vListnerPos) { m_vListenerPos = vListnerPos; }
private:
	void LoadSoundFile();
	void LoadSoundFileRecursive(const char* pPath);
private:
	static CSoundMgr* m_pInstance;

	// 사운드 리소스 정보를 갖는 객체 (64비트 FMOD::Sound 포인터)
	std::map<std::wstring, FMOD::Sound*> m_mapSound;

	// FMOD::Channel : 재생하고 있는 사운드를 관리할 객체
	FMOD::Channel* m_pChannelArr[MAXCHANNEL];

	// 사운드, 채널 객체 및 장치를 관리하는 객체 (64비트 FMOD::System)
	FMOD::System* m_pSystem;

	_vec3   m_vListenerPos;
	static constexpr _float	m_fMaxDistance = 50.f;
};

END
