#pragma once
#include "CGameObject.h"
#include "CInteractMgr.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CFollower_AI;
class CTriggerPoint;

class CFollower : public CGameObject
{
public:
	enum FOLLOWER_STATE {	FOLLOWER_IDLE,		FOLLOWER_RUN,		FOLLOWER_DANCE,		FOLLOWER_CHEER,		FOLLOWER_TRANSFORM,
							FOLLOWER_UNCONVERT,	FOLLOWER_CONVERT,	FOLLOWER_ACTION,	FOLLOWER_RECRUIT,	FOLLOWER_END };
	enum FOLLOWER_WORK { FW_NONE, FW_WOOD, FW_ROCK, FW_BUILD, FW_EAT, FW_PRAY, FW_END };

private:
	explicit	CFollower(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CFollower(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CFollower(const CFollower& rhs);
	virtual		~CFollower();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject);

	void				WaitForCommand();
	void				SetCommand(const FOLLOWER_WORK eWork);
	FOLLOWER_STATE		Get_State() { return m_eCurState; }

private:
	HRESULT				Add_Component();

	void				Ready_Variable();
	void				Ready_Event();
	void				Check_Frame();
	void				Move_Frame(const _float& fTimeDelta);
	void				Set_Texture();
	void				Update_State();
	void				Check_Work();
	void				Execute_Work(const _float& fTimeDelta);
	void				ReTarget() { m_ePreWork = FW_NONE; }

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCollider*		m_pColliderCom;
	CFollower_AI*			m_pAICom;

	_vec3				m_vPos;

	// 스프라이트 관련
	FOLLOWER_STATE	m_ePreState;
	FOLLOWER_STATE	m_eCurState;
	_float			m_fFrame;
	_float			m_fFrameEnd;
	_float			m_fFrameSpeed;
	_matrix			m_matTex;
	_uint			m_iTexIdx;
	wstring			m_strProtoKey;

	// 스테이터스 관련
	_float			m_fGroundY;
	FOLLOWER_WORK	m_ePreWork;
	FOLLOWER_WORK	m_eCurWork;
	_vec3			m_vLerpPos;
	_uint			m_iRecruitState;

	// 상호작용 관련
	_float		m_fWorkSpeed;
	_vec3		m_vWorkPos;
	_bool		m_bWorking = false;

	//트리거 관련
	CTriggerPoint* m_pTrigger = nullptr;

	static constexpr _float FW_DEFAULT_WORK_SPEED = 0.03f;
	static constexpr _float FW_DEFAULT_WORK_GAP_TIME = 1.f;

public:
	static CFollower*	Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, const wstring& pProtoTexKey);
	static CFollower* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, const wstring& pProtoTexKey, const _vec3& vPos, FOLLOWER_STATE eState = FOLLOWER_RECRUIT);

private:
	virtual void		Free();
};
