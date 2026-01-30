#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CInvenPlayer :
	public CUi
{
private:
	explicit CInvenPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInvenPlayer();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CInvenPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale);

public:
	void Set_ParentPos(_vec3 _vParentPos) { m_vParentPos = _vParentPos; }
private:
	virtual			void		Free();
	HRESULT						Add_Component();
	void		Move_Frame(const _float& fTimeDelta);
private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec3			m_vParentPos;
	_vec3			m_vLocalPos;
	_vec3			m_vWorldPos;

	_float m_fFrameSpeed;
	_float m_fFrameEnd;
	_float m_fFrame;

	float m_fScale;
	_vec3 m_vPos;
	_bool m_bRender;
};

