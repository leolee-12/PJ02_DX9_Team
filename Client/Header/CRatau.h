#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CRatau : public CGameObject
{
public:
	enum RATAU_STATE { RATAU_IDLE, RATAU_TALK, RATAU_ENTER, RATAU_EXIT, RATAU_END };

private:
	explicit	CRatau(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CRatau(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CRatau(const CRatau& rhs);
	virtual		~CRatau();

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
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCollider*	m_pColliderCom;

	_vec3				m_vPos;

	// 스프라이트 관련
	RATAU_STATE	m_ePreState;
	RATAU_STATE	m_eCurState;
	_float		m_fFrame;
	_float		m_fFrameEnd;
	_float		m_fFrameSpeed;
	_matrix		m_matTex;

	// 스테이터스 관련
	_float		m_fGroundY;


public:
	static CRatau* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void		Free();
};
