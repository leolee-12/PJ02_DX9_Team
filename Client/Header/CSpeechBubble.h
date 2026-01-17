#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CSpeechBubble :
	public CUi
{
private:
	explicit CSpeechBubble(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSpeechBubble();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	void			Active() { m_bActive = true; }
	void			UnActive() { m_bActive = false; }

public:
	static CSpeechBubble* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, _vec2 _vScale);

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	_bool m_bActive;
	_vec3 m_vTargetPos;

	_vec2 m_vScale;
};

