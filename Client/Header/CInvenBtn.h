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
	static CInvenBtn* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale);

private:
	virtual			void		Free();
	void						Check_CusorColl();
	HRESULT						Add_Component();

private:
	CRcTex*			m_pBufferCom;
	CTransform*		m_pTransformCom;
	CTexture*		m_pTextureCom;

	CFontUIOrtho*	m_pName;

	_vec2			m_vScreenPos;
	_vec2			m_vHitHalfScale;

	_vec3			m_vPos;
	float			m_fScale;
	_int			m_iPage;
	_bool			m_bRender;
};

