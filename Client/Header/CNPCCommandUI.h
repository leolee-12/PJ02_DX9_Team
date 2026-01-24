#pragma once
#include "CUi.h"


namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CNPCCommandUIBack;
class CWorkWoodUI;
class CWorkRockUI;
class CRestUI;
class CLookingEyeUI;
class CFollower;

class CNPCCommandUI :
	public CUi
{
private:
	explicit CNPCCommandUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CNPCCommandUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CNPCCommandUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);

private:
	void		Ready_Event();
private:
	virtual			void		Free();

private:
	CNPCCommandUIBack*	m_pBackUI;
	CWorkWoodUI*		m_pWoodUI;
	CWorkRockUI*		m_pRockUI;
	CRestUI*			m_pRestUI;
	CLookingEyeUI*		m_pLookingEyeUI;

	CFollower*			m_pOwner;

	_bool				m_bRender = false;
};

