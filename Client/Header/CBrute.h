#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CBrute : public CGameObject
{
public:
	enum BRUTE_STATE { BRUTE_IDLE, BRUTE_RUN, BRUTE_JEER, BRUTE_EXECUTE1, BRUTE_EXECUTE2, BRUTE_END };

private:
	explicit	CBrute(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CBrute(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CBrute(const CBrute& rhs);
	virtual		~CBrute();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject);

private:
	HRESULT				Add_Component();

	void				Ready_Variable();

	void				Check_Frame();
	void				Move_Frame(const _float& fTimeDelta);
	void				Set_Texture();

	void				Update_State();

	void				Ready_Event_Brute();

	void				Move_For_Execute();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCollider* m_pColliderCom;

	_vec3				m_vPos;

	// 스프라이트 관련
	BRUTE_STATE	m_ePreState;
	BRUTE_STATE	m_eCurState;
	_float		m_fFrame;
	_float		m_fFrameEnd;
	_float		m_fFrameSpeed;
	_matrix		m_matTex;

	// 스테이터스 관련
	_float		m_fGroundY;
	_float		m_fAcmlTime;

	_bool		m_bRunning = false;



public:
	static CBrute* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos);

private:
	virtual void		Free();
};
