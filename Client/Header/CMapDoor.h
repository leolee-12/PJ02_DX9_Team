#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CMapDoor : public CGameObject
{
private:
	explicit CMapDoor(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMapDoor(const CMapDoor& rhs);
	virtual ~CMapDoor();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();

public:
	void	Set_ObjectData(const Engine::OBJECTDATA& objData);

private:
	HRESULT	Add_Component();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_int				m_iTextureIndex;
	_float				m_fScale;
	_float				m_fBaseScale;

	_bool				m_bLeshyDoor = false;

public:
	static CMapDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::OBJECTDATA& objData, IMessageChannel* pMessageChannel);

private:
	virtual void Free();
};
