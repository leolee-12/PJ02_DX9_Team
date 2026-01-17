#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CTarotSeller : public CGameObject
{
public:
	enum TAROT_STATE { TAROT_IDLE, TAROT_TALK, TAROT_END };

private:
	explicit	CTarotSeller(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CTarotSeller(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CTarotSeller(const CTarotSeller& rhs);
	virtual		~CTarotSeller();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject);

private:
	HRESULT				Add_Component();

	void				Ready_Variable();
	void				Ready_Event();

	void				Check_Frame();
	void				Move_Frame(const _float& fTimeDelta);
	void				Set_Texture();

	void				Update_State();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_vec3				m_vPos;

	// 스프라이트 관련
	TAROT_STATE	m_ePreState;
	TAROT_STATE	m_eCurState;
	_float		m_fFrame;
	_float		m_fFrameEnd;
	_float		m_fFrameSpeed;
	_matrix		m_matTex;

	// 스테이터스 관련
	_float		m_fGroundY;


public:
	static CTarotSeller* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos);

private:
	virtual void		Free();
};
