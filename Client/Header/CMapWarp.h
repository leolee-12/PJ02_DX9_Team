#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CCollider;
}

class CWarp;

class CMapWarp : public CGameObject
{
private:
	explicit CMapWarp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMapWarp(const CMapWarp& rhs);
	virtual ~CMapWarp();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) override;

public:
	void				Set_Warps(CWarp* Warp1, CWarp* Warp2);

	CWarp*				Get_OtherWarp(CWarp* curWarp);

private:
	HRESULT				Add_Component();

private:
	pair<CWarp*, CWarp*> Warps;

	bool bRender = true;

public:
	static CMapWarp* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, _vec3 Warp1Pos, _uint Warp1Dir, _vec3 Warp2Pos, _uint Warp2Dir);
	
private:
	virtual void Free();
};

