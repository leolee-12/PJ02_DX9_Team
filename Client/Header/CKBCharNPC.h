#pragma once
#include "CUi.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CKBCharNPC : public CUi
{
public:
	enum KBCHARSTATE { KBC_IDLE, KBC_PLAY_DICE, KBC_TAKE_DICE, KBC_WIN_GAME, KBC_WIN_GAME_LOOP, KBC_LOSE_DICE, KBC_LOSE_GAME, KBC_LOSE_GAME_LOOP, KBC_END };

private:
	explicit CKBCharNPC(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKBCharNPC();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject);

	void				Set_State(KBCHARSTATE eState) { m_eCurState = eState; }
	KBCHARSTATE			Get_State() const { return m_eCurState; }

private:
	HRESULT				Add_Component();
	void				Ready_Variable();

	void				Check_Frame();
	void				Move_Frame(const _float& fTimeDelta);
	void				Set_Texture();

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform*	m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

	_vec3				m_vScreenPos;

	// 스프라이트 관련
	KBCHARSTATE			m_ePreState;
	KBCHARSTATE			m_eCurState;
	_float				m_fFrame;
	_float				m_fFrameEnd;
	_float				m_fFrameSpeed;
	_matrix				m_matTex;

public:
	static CKBCharNPC* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos);

private:
	virtual void		Free();
};
