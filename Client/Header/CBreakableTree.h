#pragma once
#include "CGameObject.h"
#include "IInteractable.h"

namespace Engine
{
	class CTransform;
	class CTexture;
	class CRcTex;
	class CCollider;
}

class CResourceWorkBar;

// =====================================================
//	CBreakableTree
// =====================================================

class CBreakableTree : public CGameObject, public IInteractable
{
private:
	explicit CBreakableTree(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBreakableTree(const CBreakableTree& rhs);
	virtual ~CBreakableTree();

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
	void	Set_Texture();
	void	Update_WorkBar(const _float& fTimeDelta);
	void	Create_Item();

private:
	Engine::CRcTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCollider*		m_pColliderCom;

	_int				m_iTextureIndex;
	_float				m_fFrame;
	_float				m_fFrameSpeed;
	_float				m_fFrameEnd;
	_matrix				m_matTex;
	_float				m_fScale;

	// 상호작용 관련
	_float					m_fPreWorkGauge;
	_float					m_fWorkGauge;
	CResourceWorkBar*		m_pWorkBar;

	static constexpr _float MAX_WORK_GAUGE = 1.f;

public:
	static CBreakableTree* Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::OBJECTDATA& objData, IMessageChannel* pMessageChannel);

private:
	virtual void Free();
};
