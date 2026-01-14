#pragma once
#include "CGameObject.h"
#include "Bishops_Enum.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTextureSet;
}

class CFontUI;
class CSpeechBubble;

class CBishop_Leshy :
	public CGameObject
{
private:
	explicit CBishop_Leshy(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBishop_Leshy();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CBishop_Leshy* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, const Engine::SPAWNDATA& tSpawndata);

	void		Update_State();
	void        Update_Frame(const _float& fTimeDelta);
	void		Set_State(Bishops::BISHOPSSTATE eState) { m_eCurState = eState; }

private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();
	void						Ready_Event();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTextureSet* m_pTextureCom;

	CFontUI* m_pFontUI;
	CSpeechBubble* m_pSpeechBubble;

	wstring      m_strStateKey;
	_int		 m_iFrame;
	_int		 m_iFrameEnd;

	_vec3		 m_vPos;

	Bishops::BISHOPSSTATE  m_eCurState;
	Bishops::BISHOPSSTATE  m_ePreState;

	Engine::SPAWNDATA      m_tSpawndata;
};

