#pragma once
#include "CUi.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CTarotInfo;

class CTarotCard : public CUi
{
public:
	enum TAROTCARD_STATE { TCS_INTRO, TCS_SHOWING, TCS_END };
	enum TAROTCARD_TYPE { TCT_HEART, TCT_SPEED, TCT_END };

private:
	explicit	CTarotCard(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CTarotCard(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel);
	explicit	CTarotCard(const CTarotCard& rhs);
	virtual		~CTarotCard();

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

	void				Check_CusorColl();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	
	_vec3 m_vScreenPos;

	// 스프라이트 관련
	TAROTCARD_STATE	m_ePreState = TCS_END;
	TAROTCARD_STATE	m_eCurState = TCS_INTRO;
	_float		m_fFrame;
	_float		m_fFrameEnd;
	_float		m_fFrameSpeed;
	_matrix		m_matTex;

	TAROTCARD_TYPE m_eType = TCT_END;

	_vec2		m_vHitHalfScale = { 100.f, 150.f };

	_bool		m_bWait = false;

	CTarotInfo* m_pTarotInfo = nullptr;


public:
	static CTarotCard* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel, _vec3 vPos, TAROTCARD_TYPE eType);

private:
	virtual void		Free();
};
