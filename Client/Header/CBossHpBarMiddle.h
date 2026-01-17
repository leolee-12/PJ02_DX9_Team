#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CBossHpBarMiddle :
	public CUi
{
private:
	explicit CBossHpBarMiddle(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBossHpBarMiddle();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CBossHpBarMiddle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale);

	void Set_Radio(_float _Radio) { m_fRatio = _Radio; }
	void Set_Hp(_float _fHp) { m_fCurHp = _fHp; }
	void InitHp(_float _fMaxHp, _float _fCurHp) { m_fMaxHp = _fMaxHp; m_fCurHp = _fCurHp; }
private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	// 비율 1.0f~ 0.0f;
	_float m_fRatio;
	_float m_fCurHp;
	_float m_fMaxHp;

	_vec3 m_vPos;
	float m_fScale;
};

