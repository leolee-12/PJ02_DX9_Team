#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CPlayer : public CGameObject
{
public:
	enum PLAYERSTATE { PS_IDLE, PS_RUN, PS_ROLL, PS_ATTACK, PS_WORK, PS_TALK, PS_INTROIDLE, PS_INTRORUN, PS_INTROKNEE, PS_END };

private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();
	void			Key_Input(const _float& fTimeDelta);
	void			Set_OnTerrain();
	_vec3			Picking_OnTerrain();

	void			Check_Frame();
	void			Move_Frame(const _float& fTimeDelta);

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;

	PLAYERSTATE		m_ePreState;
	PLAYERSTATE		m_eCurState;
	_float			m_fFrame;
	_float			m_fFrameSpeed;
	_float			m_fFrameEnd[PS_END];

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void Free();

};

