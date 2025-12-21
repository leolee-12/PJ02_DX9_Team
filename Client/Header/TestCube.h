#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CCubeTex;
}


class TestCube : public CGameObject
{
private:
	explicit TestCube(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit TestCube(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit TestCube(const TestCube& rhs);
	virtual ~TestCube();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CCubeTex* m_pCubeTexCom;

	_vec3			  m_vPos;

public:
	static TestCube* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void Free();

};

