#pragma once
#include "CBase.h"
#include "CGameObject.h"
#include "Engine_Define.h"
#include "CPlayer.h"
#include "CGauge.h"
#include "CPlayerHP.h"


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
	void		 Update_PersistnetMgr(const _float fTimeDelta);

public:
	Engine::CTransform* Get_PlayerTransform();
	CPlayer* Get_Player() { return m_pPlayer; }
	CGauge* Get_Gauge() { return m_pGauge; }
	CPlayerHP* Get_PlayerHPUI() { return m_pPlayerHPUI; }

	void	   Set_GaugeFontRender(_bool bBool);




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
	void	Update_PlayerHp();
	void	Update_PlayerGage();

private:
	CPlayer*	m_pPlayer;
	CGauge*		m_pGauge;
	CPlayerHP*	m_pPlayerHPUI;


private:
	static CPersistentMgr* m_pInstance;

private:
	virtual void		Free();
};
