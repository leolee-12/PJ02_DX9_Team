#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CWorkWoodUI :
	public CUi
{
private:
	explicit CWorkWoodUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWorkWoodUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CWorkWoodUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, _vec3 _vPos, float _fScale);
	void	Set_Pos(_vec3 _vPos) { m_vPos = _vPos; }

private:
	virtual			void		Free();
	void    Check_CusorColl();
	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec2	m_vScreenPos;
	_vec2   m_vHitHalfScale;

	_vec3 m_vPos;
	float m_fScale;
};

