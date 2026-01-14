#pragma once
#include "CGameObject.h"

namespace Warp
{
	enum WarpScene
	{
		TO_GATEWAY,
		TO_DUNGEON,
		TO_VILLAGE,
		TO_REALDUNGEON,
		WARP_END,
	};

	enum WarpDir
	{
		WARP_LEFT,
		WARP_RIGHT,
		WARP_UP,
		WARP_DOWN,
		WARP_DIR_END,
	};
}
using namespace Warp;


namespace Engine
{
	class CTransform;
	class CCollider;
}

class CMapWarp;

class CWarp : public CGameObject
{
private:
	explicit CWarp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWarp(const CWarp& rhs);
	virtual ~CWarp();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) override;

public:
	void	Set_Position(float x, float z);
	void	Set_Scale(float size);
	void	RegisterToManager();

	void	Set_Outer(CMapWarp* outer) { MapWarpOuter = outer; }
	void	Set_WarpDir(WarpDir dir) { m_WarpDir = dir; }
	void	Set_WarpPos(_vec3 pos) { m_WarpPosition = pos; }
	void	Set_bRender(bool bRend) { bRender = bRend; }

	CWarp*	Get_OtherWarp();
	_vec3	Get_Pos();
	_vec3	Get_WarpPos() { return m_WarpPosition; }

private:
	HRESULT	Add_Component();

private:
	Engine::CTransform* m_pTransformCom;
	Engine::CCollider* m_pColliderCom;

	bool bRender = true;

	CMapWarp* MapWarpOuter = nullptr;
	WarpDir m_WarpDir = WARP_DIR_END;

	_vec3 m_WarpPosition = { 0,0,0 }; // 워프가 되어야할 장소

public:
	static CWarp* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel,
						CMapWarp* outer, _vec3 pos, WarpDir dir);

private:
	virtual void Free();
};

