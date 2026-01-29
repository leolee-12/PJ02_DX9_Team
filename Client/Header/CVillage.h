#pragma once

#include "CScene.h"
#include "CFollower.h"
#include <queue>

class CCookingUIController;
class CBuilding;

class CFontUIOrtho;
class CSpeechBubbleOrtho;
class CSelectionArrow;
class CBuildingCraftCtrl;

// 팔로워 스폰 작업 구조체
struct FOLLOWER_SPAWN_WORK
{
	wstring			strProtoTexKey;
	_vec3			vPos;
	CFollower::FOLLOWER_STATE	eState;
	_bool			bUseTransform;	// 위치/상태 사용 여부

	FOLLOWER_SPAWN_WORK()
		: strProtoTexKey(L"")
		, vPos(0.f, 0.f, 0.f)
		, eState(CFollower::FOLLOWER_IDLE)
		, bUseTransform(false)
	{}

	FOLLOWER_SPAWN_WORK(const _tchar* pKey)
		: strProtoTexKey(pKey)
		, vPos(0.f, 0.f, 0.f)
		, eState(CFollower::FOLLOWER_IDLE)
		, bUseTransform(false)
	{}

	FOLLOWER_SPAWN_WORK(const _tchar* pKey, const _vec3& _vPos, CFollower::FOLLOWER_STATE _eState = CFollower::FOLLOWER_RECRUIT)
		: strProtoTexKey(pKey)
		, vPos(_vPos)
		, eState(_eState)
		, bUseTransform(true)
	{}
};

class CVillage : public CScene
{
private:
	explicit CVillage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CVillage();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);

	HRESULT			Ready_Light();
	void			Ready_Event();

	void			Ready_Event_Village();

	void			Key_Input_Village();
	void			Key_Input_Village_Debug();

	void			Process_FollowerSpawnQueue(const _float& fTimeDelta);
	void			Add_FollowerSpawnWork(const FOLLOWER_SPAWN_WORK& tWork);

	void			Update_Building(const _float& fTimeDelta);
	void			LateUpdate_Building(const _float& fTimeDelta);

	_vec3			Compute_GirdCoord(const _vec3& vPos);

	void			Select_Key_Input();

public:
	static CVillage* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free();

private:
	_bool	m_bLeshyDungeonFlag = false;
	_bool	m_bKnuckleBoneFlag = false;
	_bool	m_bCookingFlag = false;
	CCookingUIController* m_pCookingUI;

	_bool	m_bReEnterFlag = false;

	// 빌딩 관련 변수
	_bool	m_bBuildingFlag = false;
	CBuilding* m_pCurBuilding = nullptr;

	// 선택지 관련
	CFontUIOrtho* m_pLeftSelect;
	CFontUIOrtho* m_pRightSelect;

	CSpeechBubbleOrtho* m_pSpeechBubble;

	CSelectionArrow* m_pSelectionArrow;

	CBuildingCraftCtrl* m_pBuildingCraftCtrl;	// 디버그용 캐싱

	_bool		  m_bShowSelect = false;
	_uint		  m_iSelectSlot = 0;

	// 팔로워 스폰 작업 큐
	queue<FOLLOWER_SPAWN_WORK>	m_queueFollowerSpawn;
	_float						m_fSpawnTimer = 0.f;
	static constexpr _float		FOLLOWER_SPAWN_DELAY = 0.5f;
	static constexpr _float		BUILDING_GRIDSIZE	 = 2.f;
};
