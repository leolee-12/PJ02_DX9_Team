#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CEffect : public CGameObject
{
public:
	enum EFFECT_TYPE { EF_SPRITE, EF_PARTICLE, EF_TRAIL, EF_DISTORTION, EF_SCREEN, EF_INDICATOR, EF_END };
	enum EFFECT_STATE { ES_READY, ES_PLAY, ES_LOOP, ES_FINISH };

protected:
	explicit	CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CEffect(const CEffect& rhs);
	virtual		~CEffect();

public:
	void				Set_TextureIndex(const _uint& iTexIdx)	{ m_iTexIdx = iTexIdx; }
	void				Set_Owner(CGameObject* pOwner)			{ m_pOwner = pOwner; }
	void				Set_Scale(const _vec3& vScale)			{ m_vScale = vScale; }
	void				Set_Dead()								{ m_iHp = 0; }

	virtual HRESULT		Ready_GameObject() PURE;
	virtual _int		Update_GameObject(const _float& fTimeDelta) PURE;
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta) PURE;
	virtual void		Render_GameObject() PURE;

	// 필수
	virtual void    Play()	PURE;
	virtual void    Stop()	PURE;
	virtual void    Reset()	PURE;

	// 선택
	virtual void    OnPlay() {}         // 재생 시작 시
	virtual void    OnStop() {}         // 정지 시
	virtual void    OnFinish() {}       // 완료 시
	virtual void    OnLoop() {}         // 루프 시

	// 유틸리티
	void            AttachTo(CGameObject* pOwner, const _vec3& vOffset = { 0,0,0 });
	void            Detach();
	bool            IsPlaying() const { return m_eState == ES_PLAY || m_eState == ES_LOOP; }

protected:
	HRESULT			Add_Component();

	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	wstring				m_strProtoTexKey;
	_uint				m_iTexIdx;

	EFFECT_TYPE     m_eType;
	EFFECT_STATE    m_eState;

	_vec3			m_vScale;
	_float          m_fLifeTime;        // 총 지속 시간
	_float          m_fAccTime;			// 경과 시간
	_bool           m_bLoop;            // 루프 여부

	_vec3           m_vOffset;          // 부모로부터의 오프셋
	CGameObject*	m_pOwner;           // 부착 대상 (nullptr = 월드)

	// 복사생성 관련
	_bool			m_bClone = false;

public:
	virtual CEffect*	Clone() PURE;

	virtual void		Free();
};
