#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CResourceHistoryItem :
	public CUi
{
private:
	explicit CResourceHistoryItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CResourceHistoryItem();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CResourceHistoryItem* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale, _int _iPage);
	void	Set_Pos(_vec3 _vPos) { m_vPos = _vPos; }
private:
	virtual			void		Free();

	HRESULT						Add_Component();


private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_int		m_iPage;
	_vec3 m_vPos;
	float m_fScale;
};

