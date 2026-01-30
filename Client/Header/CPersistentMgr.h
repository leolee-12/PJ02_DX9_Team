#pragma once
#include "CBase.h"
#include "CGameObject.h"
#include "Engine_Define.h"
#include "CPlayer.h"
#include "CGauge.h"
#include "CPlayerHP.h"
#include "CVillage.h"
#include "CResourceHistoryController.h"
#include "CPlayerWeaponUI.h"
#include "CInventory.h"
#include "CCursor.h"

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
	CCursor* Get_Cursor() { return m_pCursor; }
	CPlayer* Get_Player() { return m_pPlayer; }
	CGauge* Get_Gauge() { return m_pGauge; }
	CPlayerHP* Get_PlayerHPUI() { return m_pPlayerHPUI; }
	CVillage* Get_Village() { return m_pVillage; }
	CResourceHistoryController* Get_ResourceHistory() { return m_pResourceHistoryUI; }
	CPlayerWeaponUI* Get_WeaponUIfirst() { return m_pWeaponUIfirst; }
	CPlayerWeaponUI* Get_WeaponUIsecond() { return m_pWeaponUIsecond; }
	CInventory* Get_Inventory() { return m_pInventory; }

	void	Set_Village(CVillage* pVillage)
	{
		if (m_pVillage == nullptr)
		{
			m_pVillage = pVillage;
			// AddRef 제거 - Create()의 소유권을 그대로 인수
		}
	}

	void	   Set_GaugeFontRender(_bool bBool);

	_bool	   isArchived_Village()
	{
		if (m_pVillage)
		{
			return true;
		}
		return false;
	}

	// 마을 재진입 플래그
	void	Set_VillageReentry(_bool b) { m_bVillageReentry = b; }
	_bool	Is_VillageReentry() { return m_bVillageReentry; }




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
	void	Update_PlayerWeaponUI();

private:
	CPlayer*			m_pPlayer				= nullptr;
	CGauge*				m_pGauge				= nullptr;
	CPlayerHP*			m_pPlayerHPUI			= nullptr;
	CVillage*			m_pVillage				= nullptr;
	CResourceHistoryController* m_pResourceHistoryUI	= nullptr;
	CInventory*			m_pInventory			= nullptr;
	CCursor*			m_pCursor				= nullptr;

	CPlayerWeaponUI*    m_pWeaponUIfirst = nullptr;
	CPlayerWeaponUI*	m_pWeaponUIsecond = nullptr;

	_bool				m_bVillageReentry		= false;

private:
	static CPersistentMgr* m_pInstance;

private:
	virtual void		Free();
};
