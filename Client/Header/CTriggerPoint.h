#pragma once
#include "CGameObject.h"
#include "Trigger_Enum.h" 

namespace Engine
{
	class CTransform;
	class CCollider;
}

class CTriggerPoint : public CGameObject
{
private:
	explicit CTriggerPoint(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTriggerPoint(const CTriggerPoint& rhs);
	virtual ~CTriggerPoint();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) override;

public:
	_int				Get_TID_for_int() { return _int(m_eTID); }

	void				Activate();

public:
	static CTriggerPoint* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, _vec3 vPos, _vec3 vHalfSize, Trigger::TRIGGERID eTID, const wstring& strTriggerName, _bool bPassive = false);

private:
	HRESULT	Add_Component();

private:
	virtual void Free();

private:
	Engine::CTransform* m_pTransformCom;
	Engine::CCollider* m_pColliderCom;

	_vec3 m_vColHalfSize = {};
	_vec3 m_vPos = {};

	Trigger::TRIGGERID  m_eTID;
	wstring				m_strTriggerName;
	_bool				m_bPassive = false;
};

