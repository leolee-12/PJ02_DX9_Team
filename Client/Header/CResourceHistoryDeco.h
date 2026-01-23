#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CResourceHistoryDeco :
	public CUi
{
private:
	explicit CResourceHistoryDeco(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CResourceHistoryDeco();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CResourceHistoryDeco* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale);
	void	Set_Pos(_vec3 _vPos) { m_vPos = _vPos; }
private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_vec3 m_vPos;
	float m_fScale;
};

