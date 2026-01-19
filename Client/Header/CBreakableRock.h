#pragma once
#include "CGameObject.h"
#include "IInteractable.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

// ===================================================
//	CBreakableRock
// ===================================================

class CBreakableRock : public CGameObject, public IInteractable
{
private:
	explicit CBreakableRock(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBreakableRock(const CBreakableRock& rhs);
	virtual ~CBreakableRock();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) override;

	virtual void	Add_WorkGauge(_float fWork) { m_fWorkGauge += fWork; }
	virtual _float	Get_WorkGauge() const { return m_fWorkGauge; }
	virtual _bool	Is_WorkComplete() const { return m_fWorkGauge >= MAX_WORK_GAUGE; }

public:
	void	Set_ObjectData(const Engine::OBJECTDATA& objData);
	void	Check_Status();

private:
	HRESULT	Add_Component();
	void	React(const _vec3& vObjPos);
	void	Update_VertexSway(const _float& fTimeDelta);

private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCollider*	m_pColliderCom;

	_int				m_iTextureIndex;
	_float				m_fScale;
	_float				m_fBaseScale;

	// Vertex sway variables (replaced old Interaction vars)
	_float				m_fPhase;
	_float				m_fWindSpeed;
	_float				m_fWindStrength;
	_float				m_fAccTime;
	_float				m_fReactStrength;
	_vec3				m_vReactDir;

	// 상호작용 관련
	_float				m_fWorkGauge;

	static constexpr _float MAX_WORK_GAUGE = 1.f;

public:
	static CBreakableRock* Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::OBJECTDATA& objData, IMessageChannel* pMessageChannel);

private:
	virtual void Free();
};
