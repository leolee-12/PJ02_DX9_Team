#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CSelectionArrow :
	public CUi
{
private:
	explicit CSelectionArrow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSelectionArrow();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	void						Set_Dir(_uint iDir) { m_iDir = iDir; }
	void						Active() { m_bActive = true; }
	void						UnActive() { m_bActive = false; }

public:
	static CSelectionArrow* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos);

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();
	void						Turn();


private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_bool m_bActive = false;

	_vec3 m_vPos;
	_uint m_iDir;
	_float m_fCurAngle = 0.f;
};

