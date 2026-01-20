#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CMonsterHpFront :
	public CUi
{
private:
	explicit CMonsterHpFront(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonsterHpFront();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CMonsterHpFront* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos);

	void Set_Radio(_float _fRatio) { m_fRatio = _fRatio; }
	void Set_Hp(_float _fHp) { m_fCurHp = _fHp; }
	void Set_LerpHp(_float fHp) { m_fLerpHp = fHp; }
	void InitHp(_float _fMaxHp, _float _fCurHp) { m_fMaxHp = _fMaxHp; m_fCurHp = _fCurHp; }

	void			Set_Pos(const _vec3& vPos);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

	HRESULT 					Ready_PixelShader();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	// 비율 1.0f~ 0.0f;
	_float m_fRatio;
	_float m_fCurHp;
	_float m_fLerpHp;
	_float m_fMaxHp;

	_vec3 m_vPos;

	LPDIRECT3DPIXELSHADER9 m_pPixelShader;
};

