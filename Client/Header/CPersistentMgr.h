#pragma once
#include "CBase.h"
#include "CGameObject.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CPersistentMgr : public CBase
{
private:
	explicit CPersistentMgr();
	explicit CPersistentMgr(const CPersistentMgr& rhs) = delete;
	CPersistentMgr operator = (const CPersistentMgr&) = delete;
	virtual ~CPersistentMgr();

public:
	HRESULT		 Ready_GlobalObjects(LPDIRECT3DDEVICE9 pGraphicDev);
	CGameObject* Get_GlobalObjects(GOBJID eGOBJID);

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
	unordered_map<GOBJID, CGameObject*>  m_hmapGlobalObjects;
	vector<CGameObject*>				 m_vecReleaseQueue;

private:										
	static CPersistentMgr* m_pInstance;

private:
	virtual void		Free();
};

END