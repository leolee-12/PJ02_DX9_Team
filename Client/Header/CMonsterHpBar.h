#pragma once
#include "CUi.h"


namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CMonsterHpFront;
class CMonsterHpBarCover;

class CMonsterHpBar :
	public CUi
{
private:
	explicit CMonsterHpBar(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonsterHpBar();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CMonsterHpBar* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fMaxHp, const _vec3& vTargetPos);
	_bool Check_CookingResult();

	void Set_Hp(_float _fCurrtHp) { m_fCurHp = _fCurrtHp; }
	_float Get_Hp() const { return m_fCurHp; }
	void Set_MaxHp(_float _fMaxHp) { m_fMaxHp = _fMaxHp; }
	void ApplyDamage(_float _fDamage);

	float Lerp(float _fStart, float _fEnd, float t) { return _fStart + (_fEnd - _fStart) * t; }

	void Update_Hp(_uint Hp) { m_fCurHp = _float(Hp); }

	void Calcute_WorldPos();
	void Set_TargetPos(const _vec3& vTergetPos) { m_vTargetPos = vTergetPos; }

	void		Active();
	void		UnActive();

private:
	virtual			void		Free();

private:
	vector<CGameObject*> m_vecHpBarUI;

	CMonsterHpFront* m_pMonsterHpBarFront;
	CMonsterHpBarCover* m_pMonsterHpBarCover;

	_float m_fLerpTime;
	_float m_fLefpPrevHp;
	_float m_fMaxHp;
	_float m_fPrevHp;
	_float m_fCurHp;
	_float m_fRadio;

	_vec3 m_vTargetPos;
	_vec3 m_vCalcutePos;
	_vec3 m_vCoverPos;

	_bool m_bActive = false;
};

