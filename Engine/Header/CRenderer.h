#pragma once

#include "CBase.h"
#include "CGameObject.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CCollider;

class ENGINE_DLL CRenderer : public CBase
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer();
	virtual ~CRenderer();

public:
	void		Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
	void        Add_ColliderDebugGroup(CCollider* pCollider);
	void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void		Clear_RenderGroup();
	void        Add_TestCollider(const AABB& tAABB, _int iFrame);

	void		Set_RenderFont(_bool b) { m_bRenderFont = b; }

private:
	void			Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Tile(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Floor(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Font(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_ColliderDebug(LPDIRECT3DDEVICE9& pGraphicDev);

private:
	typedef struct tagTestCollider
	{
		AABB		tAABB;
		_int		iFrame;
	}TESTCOL;

private:
	list<CGameObject*>			m_RenderGroup[RENDER_END];
	vector<CCollider*>			m_vecColDebugGroup;
	vector<TESTCOL>				m_vecTestColliders;

	_bool						m_bRenderFont = true;

private:
	virtual void		Free();

};

END
