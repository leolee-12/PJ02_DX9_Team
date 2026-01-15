#pragma once
#include "CBase.h"
#include "CGameObject.h"
#include "Engine_Define.h"
#include "CPlayer.h"

namespace Engine
{
	class CTransform;
}

class CPersistentMgr : public CBase
{
private:
	explicit CPersistentMgr();
	explicit CPersistentMgr(const CPersistentMgr& rhs) = delete;
	CPersistentMgr operator = (const CPersistentMgr&) = delete;
	virtual ~CPersistentMgr();

public:
	HRESULT		 Ready_GlobalObjects(LPDIRECT3DDEVICE9 pGraphicDev);
	Engine::CTransform* Get_PlayerTransform();
	CPlayer* Get_Player() { return m_pPlayer; }


public:
	static CPersistentMgr* GetInstance()
	{
		if (m_pInstance == nullptr) {
			m_pInstance = new CPersistentMgr;
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
	CPlayer* m_pPlayer;


private:
	static CPersistentMgr* m_pInstance;

private:
	virtual void		Free();
};
