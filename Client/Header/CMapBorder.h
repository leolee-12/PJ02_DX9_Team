#pragma once
#include "CGameObject.h"

BEGIN(Engine)

class CTransform;
class CCollider;

END

class CMapBorder : public Engine::CGameObject
{
private:
	explicit CMapBorder(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMapBorder();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int    Update_GameObject(const _float& fTimeDelta) override;
	virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void    Render_GameObject() override;

public:
	void Set_TilePosition(_int tileX, _int tileZ);

private:
	HRESULT Add_Component();

private:
	Engine::CTransform* m_pTransformCom;
	Engine::CCollider* m_pColliderCom;

	_int m_iTileX;
	_int m_iTileZ;

public:
	static CMapBorder* Create(LPDIRECT3DDEVICE9 pGraphicDev, _int tileX, _int tileZ);

private:
	virtual void Free() override;
};

