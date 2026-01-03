#pragma once
#include "CGameObject.h"
#include "Engine_Struct.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CGrass : public CGameObject
{
private:
	explicit CGrass(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGrass(const CGrass& rhs);
	virtual ~CGrass();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();

public:
	void	Set_ObjectData(const Engine::OBJECTDATA& objData);

private:
	HRESULT	Add_Component();
	void	React_ToPlayer(const _float& fTimeDelta);

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

	_int				m_iTextureIndex;
	_float				m_fScale;
	_float				m_fBaseScale;

	// Interaction
	_float				m_fSwayAngle;
	_float				m_fSwaySpeed;
	_bool				m_bIsSwaying;

public:
	static CGrass* Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::OBJECTDATA& objData);

private:
	virtual void Free();
};
