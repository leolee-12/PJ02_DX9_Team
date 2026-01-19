#pragma once
#include "CGameObject.h"
#include "Bishops_Enum.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTextureSet;
	class CTexture;
}

class CFontUI;
class CSpeechBubble;

class CBishop_Leshy :
	public CGameObject
{
public:
	enum LESHYSTATE {LS_ENTER, LS_TRANS, LS_END};
private:
	explicit CBishop_Leshy(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBishop_Leshy();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			HRESULT		Ready_GameObject_Custom();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	void			Set_Wait();

public:
	static CBishop_Leshy* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, const Engine::SPAWNDATA& tSpawndata);
	static CBishop_Leshy* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, const _vec3& vPos);

	void		Update_State();
	void        Update_Frame(const _float& fTimeDelta);
	void		Set_State(Bishops::BISHOPSSTATE eState) { m_eCurState = eState; }

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();
	void						Ready_Event();

	HRESULT						Ready_Dialogue(const _vec3& vDialoguePos);

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTextureSet* m_pTextureCom;
	CTexture*	m_pLeshyTransform;
	CTexture*	m_pLeshyEnter;


	CFontUI* m_pFontUI;
	CSpeechBubble* m_pSpeechBubble;

	wstring      m_strStateKey;
	_int		 m_iFrame;
	_int		 m_iFrameEnd;

	_vec3		 m_vPos;

	Bishops::BISHOPSSTATE  m_eCurState;
	Bishops::BISHOPSSTATE  m_ePreState;
	LESHYSTATE			   m_eCurNewState;
	LESHYSTATE			   m_ePreNewState;

	Engine::SPAWNDATA      m_tSpawndata;

	_float m_fFrameTime;

	_bool  m_bNew = false;
	_bool  m_bWait = false;
};

