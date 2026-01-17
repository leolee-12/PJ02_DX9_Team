#pragma once
#include "CUi.h"


namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}

class CBossHpBarFront;
class CBossHpBarMiddle;

enum BossHpBarState
{

};

class CBossHpBar :
	public CUi
{
private:
	explicit CBossHpBar(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBossHpBar();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CBossHpBar* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fMaxHp);
	_bool Check_CookingResult();

	void Set_Render(_bool _bRender) { m_bRender = _bRender; }
	void Set_Hp(_float _fCurrtHp){ m_fCurHp = _fCurrtHp; }
	_float Get_Hp() const { return m_fCurHp; }
	void Set_MaxHp(_float _fMaxHp) { m_fMaxHp = _fMaxHp; }
	void ApplyDamage(_float _fDamage);

	float Lerp(float _fStart, float _fEnd, float t){ return _fStart + (_fEnd - _fStart) * t; }

private:
	virtual			void		Free();

private:
	vector<CGameObject*> m_vecHpBarUI;

	CBossHpBarFront* m_pBossHpBarFront;
	//CBossHpBarMiddle* m_pBossHpBarMiddle;

	_bool		m_bRender;

	_float m_fLerpTime;
	_float m_fLefpPrevHp;
	_float m_fMaxHp;
	_float m_fPrevHp;
	_float m_fCurHp;
	_float m_fRadio;
};

