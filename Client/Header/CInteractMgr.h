#pragma once
#include "CBase.h"
#include "Engine_Define.h"

namespace Engine
{
	class CGameObject;
}

class CInteractMgr : public CBase
{
	DECLARE_SINGLETON(CInteractMgr)

public:
	enum INTERACT_TYPE { NONE, WOOD, ROCK, BUILD, FOOD, PRAY, INTERACT_END };

private:
	explicit CInteractMgr();
	virtual ~CInteractMgr();

public:
	HRESULT			Ready_InteractMgr();
	void			Register_IObj(INTERACT_TYPE eType, CGameObject* pObj);
	void			Unregister_IObj(INTERACT_TYPE eType, CGameObject* pObj);
	CGameObject*	Find_Nearest(INTERACT_TYPE eType, const _vec3& vPos);
	void			Apply_Work(INTERACT_TYPE eType, const _vec3& vPos, const float& fWork);
	void			Clear_IObj();

private:
	unordered_map<INTERACT_TYPE, list<CGameObject*>> m_mapInteractables;

private:
	virtual void Free();
};
