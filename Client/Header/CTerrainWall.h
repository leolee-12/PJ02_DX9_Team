#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTerrainWallTex;
	class CTransform;
	class CTexture;
}

class CTerrainWall : public CGameObject
{
private:
	explicit CTerrainWall(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainWall(LPDIRECT3DDEVICE9 pGraphicDev, optional<_vec3> vPos, optional<_float> fRotY);
	explicit CTerrainWall(const CTerrainWall& rhs);
	virtual ~CTerrainWall();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CTerrainWallTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	optional<_vec3>		m_optPos;
	optional<_float>	m_optRotY;
public:
	static CTerrainWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, optional<_vec3> vPos = nullopt, optional<_float> fRotY = nullopt);

private:
	virtual void Free();

};

