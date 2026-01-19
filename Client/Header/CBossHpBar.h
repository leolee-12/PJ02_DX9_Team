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
class CFontUIOrtho;

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
	static CBossHpBar* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fMaxHp, const wstring& strOwnerName);
	_bool Check_CookingResult();

	void Set_Hp(_float _fCurrtHp){ m_fCurHp = _fCurrtHp; }
	_float Get_Hp() const { return m_fCurHp; }
	void Set_MaxHp(_float _fMaxHp) { m_fMaxHp = _fMaxHp; }
	void ApplyDamage(_float _fDamage);

	float Lerp(float _fStart, float _fEnd, float t){ return _fStart + (_fEnd - _fStart) * t; }

	void Update_Hp(_uint Hp) { m_fCurHp = _float(Hp); }

	void		Active();
	void		UnActive();

private:
	virtual			void		Free();

private:
	vector<CGameObject*> m_vecHpBarUI;

	CBossHpBarFront* m_pBossHpBarFront;
	//CBossHpBarMiddle* m_pBossHpBarMiddle;

	_float m_fLerpTime;
	_float m_fLefpPrevHp;
	_float m_fMaxHp;
	_float m_fPrevHp;
	_float m_fCurHp;
	_float m_fRadio;

	wstring m_strOwnerName;

	CFontUIOrtho* m_pFont;

	_bool m_bActive = false;
};

