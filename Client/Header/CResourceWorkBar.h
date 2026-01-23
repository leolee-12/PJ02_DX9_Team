#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CResourceWorkFront;
class CMonsterHpBarCover;

class CResourceWorkBar : public CUi
{
private:
	explicit CResourceWorkBar(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CResourceWorkBar();

public:
	void		Update_CurWork(_float fCurWork)			{ m_fCurWork = fCurWork; }
	void		Set_MaxWork(_float fMaxWork)			{ m_fMaxWork = fMaxWork; }
	void		Set_TargetPos(const _vec3& vTergetPos)	{ m_vTargetPos = vTergetPos; }

	_float		Get_CurWork() const						{ return m_fCurWork; }

	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject);

	_bool		Check_CookingResult();
	float		Lerp(float fStart, float fEnd, float t) { return fStart + (fEnd - fStart) * t; }
	void		Calcute_WorldPos();
	void		ApplyDamage(_float fDamage);
	void		Active();
	void		UnActive();

private:
	vector<CGameObject*> m_vecHpBarUI;

	CResourceWorkFront* m_pResourceWorkBarFront;
	CMonsterHpBarCover* m_pResourceWorkBarCover;

	_float m_fLerpTime;
	_float m_fLefpPrevHp;
	_float m_fMaxWork;
	_float m_fPrevWork;
	_float m_fCurWork;
	_float m_fRadio;

	_vec3 m_vTargetPos;
	_vec3 m_vCalcutePos;
	_vec3 m_vCoverPos;

	_bool m_bActive = false;

public:
	static CResourceWorkBar*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fMaxHp, const _vec3& vTargetPos);

private:
	virtual	void			Free();
};

