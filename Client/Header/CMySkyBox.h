#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CCubeTex;
	class CTransform;
}

/* ===================================================
	CMySkyBox

	�ܻ� �ϴ��� �������ϴ� skybox
	- 0: ������
	- 1: ������
 =====================================================*/

class CMySkyBox : public CGameObject
{
private:
	explicit CMySkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMySkyBox(const CGameObject& rhs);
	virtual ~CMySkyBox();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

public:
	void			Set_SkyType(_int iSkyType);

private:
	HRESULT			Add_Component();

private:
	Engine::CCubeTex*			m_pBufferCom;
	Engine::CTransform*			m_pTransformCom;
	DWORD						m_dwColor;

public:
	static CMySkyBox* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iSkyType = 0);

private:
	virtual void Free();
};
