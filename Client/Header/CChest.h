#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CSpeechBubble;
class CFontUI;

class CChest : public CGameObject
{
public:
	enum CHESTSTATE { CHEST_INTRO, CHEST_IDLE, CHEST_OPEN, CHEST_END };

private:
	explicit	CChest(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CChest(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CChest(const CChest& rhs);
	virtual		~CChest();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject);

public:
	void				Wait() { m_bWait = true; }


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
	Engine::CCollider* m_pColliderCom;

	_vec3				m_vPos;

	// 스프라이트 관련
	CHESTSTATE	m_ePreState;
	CHESTSTATE	m_eCurState;
	_float		m_fFrame;
	_float		m_fFrameEnd;
	_float		m_fFrameSpeed;
	_matrix		m_matTex;

	// 스테이터스 관련
	_float		m_fGroundY;

	_bool		   m_bWait = false;

	_uint		m_iItemCount;


public:
	static CChest* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos, _uint iItemCount);

private:
	virtual void		Free();
};
