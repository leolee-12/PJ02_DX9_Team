#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CFontUIOrtho;

class CInvenBtn :
	public CUi
{
private:
	explicit CInvenBtn(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInvenBtn();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CInvenBtn* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale);

public:
	void Set_ParentPos(_vec3 _vParentPos) { m_vParentPos = _vParentPos; }
	void Set_Tex(wstring _szName);
private:
	virtual			void		Free();
	void						Check_CusorColl();
	HRESULT						Add_Component();

private:
	CRcTex*			m_pBufferCom;
	CTransform*		m_pTransformCom;
	CTexture*		m_pTextureCom;

	CFontUIOrtho*	m_pName;
	wstring			m_szName;

	_vec2			m_vScreenPos;
	_vec2			m_vHitHalfScale;

	_vec3			m_vParentPos;
	_vec3			m_vLocalPos;
	_vec3			m_vWorldPos;

	_float			m_fScale;
	_bool			m_bRender;

	_int			m_iPage;
};

