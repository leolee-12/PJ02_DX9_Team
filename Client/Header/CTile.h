#pragma once
#include "CGameObject.h"
#include "Engine_Struct.h"

namespace Engine
{
	class CRcTexXZ;
	class CTransform;
	class CTexture;
}

class CTile : public CGameObject
{
private:
	explicit CTile(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTile(const CTile& rhs);
	virtual ~CTile();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();

public:
	void	Set_TileData(_int iGridX, _int iGridZ, _int iTextureId, _int iMaskFlags);

private:
	HRESULT	Add_Component();
	void	Render_Masks();

private:
	Engine::CRcTexXZ*	m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

	// Mask Texture (Down=0, Left=1, Right=2, Up=3)
	Engine::CTexture*	m_pMaskTextureCom;

	_int	m_iGridX;
	_int	m_iGridZ;
	_int	m_iTextureId;
	_int	m_iMaskFlags;

public:
	static CTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::TILEDATA& tileData);

private:
	virtual void Free();
};
