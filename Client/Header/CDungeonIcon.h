#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CDungeonIcon :
	public CUi
{
public:
	enum DICONSTATE { DI_STAGE1, DI_STAGE2, DI_STAGE3, DI_END };
private:
	explicit CDungeonIcon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDungeonIcon(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);
	virtual ~CDungeonIcon();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);


public:
	static CDungeonIcon* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, DICONSTATE eState);

private:
	virtual			void		Free();

	HRESULT						Add_Component();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom_Icon;
	CTexture* m_pTextureCom_MG;
	CTexture* m_pTextureCom_BG;

	_matrix		m_matUV;

	_bool		m_bRender;
	_vec3		m_vColor;
	DICONSTATE  m_eState = DI_END;
};

