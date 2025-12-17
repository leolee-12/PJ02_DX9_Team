#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	//class CTexture;
	class CTextureSet;
	class CCalculator;
}

class CPlayer : public CGameObject
{
public:
	enum PLAYERSTATE { PS_IDLE, PS_RUN, PS_ROLL, PS_ATTACK, PS_ACTION, PS_TALK, PS_INTROIDLE, PS_INTRORUN, PS_INTROKNEE, PS_END };
	enum DIRECTIONID { DIR_RIGHT, DIR_RD, DIR_DOWN, DIR_LD, DIR_LEFT, DIR_LU, DIR_UP, DIR_RU, DIR_END };

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
	void			Set_TextureSet();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTextureSet* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

	// 스프라이트 관련
	PLAYERSTATE		m_ePreState;
	PLAYERSTATE		m_eCurState;
	_float			m_fFrame;
	_float			m_fFrameEnd;
	_float			m_fFrameSpeed;
	_vec3			m_vNormDir[DIR_END];
	_vec3			m_vDir;

	// 캐릭터 스테이터스 관련
	_float			m_fSpeed;
	_int			m_iAttack;

	// 알파 소팅 관련
	_vec3			m_vPos;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);

private:
	virtual void Free();

};

