#pragma once
#include "CGameObject.h"
#include "IInteractable.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	//class CTexture;
	class CCollider;
}

class CResourceWorkBar;

// ===================================================
//	CShrineSpot
// ===================================================

class CShrineSpot : public CGameObject, public IInteractable
{
private:
	explicit CShrineSpot(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShrineSpot(const CShrineSpot& rhs);
	virtual ~CShrineSpot();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) override;

	virtual void	Add_WorkGauge(_float fWork) { m_fWorkGauge += fWork; }
	virtual _float	Get_WorkGauge() const { return m_fWorkGauge; }
	virtual _bool	Is_WorkComplete() const { return m_fWorkGauge >= MAX_WORK_GAUGE; }
	virtual _vec3*	Get_WorkPos(_vec3* pWorkPos) const;

public:
	void	Set_ObjectData(const Engine::OBJECTDATA& objData);
	void	Check_Status();

private:
	HRESULT	Add_Component();
	void	Update_WorkBar(const _float& fTimeDelta);

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	//Engine::CTexture* m_pTextureCom;
	Engine::CCollider* m_pColliderCom;

	_float				m_fScale;

	// 상호작용 관련
	_float				m_fPreWorkGauge;
	_float				m_fWorkGauge;
	CResourceWorkBar*	m_pWorkBar;
	_float				m_fAcmlTime = 0.f;

	static constexpr _float MAX_WORK_GAUGE = 1.f;

public:
	static CShrineSpot* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, IMessageChannel* pMessageChannel);

private:
	virtual void Free();
};
