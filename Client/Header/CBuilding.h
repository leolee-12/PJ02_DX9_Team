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


class CBuilding : public CGameObject, public IInteractable
{
public:
	enum BUILDING_TYPE { BT_DUMMY, BT_WORKSHOP, BT_COOK, BT_KNUCKLEBONE, BT_RECRUIT, BS_END };
	enum BUILDING_STATE { BS_CONSTRUCTING, BS_COMPLETE, BS_END };

private:
	explicit CBuilding(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBuilding(const CBuilding& rhs);
	virtual ~CBuilding();

public:
	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();
	virtual void		OnCollision(CGameObject* pObject) override;

	virtual void		Add_WorkGauge(_float fWork);
	virtual _float		Get_WorkGauge() const { return m_fWorkGauge; }
	virtual _bool		Is_WorkComplete() const { return m_fWorkGauge >= MAX_WORK_GAUGE; }

private:
	HRESULT	Add_Component();
	void	Change_State(BUILDING_STATE eState);
	void	Player_Interact();
	void	Set_Texture();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCollider* m_pColliderCom;

	BUILDING_TYPE	m_eBuildingType;
	BUILDING_STATE	m_eBuildingState;

	// 상호작용 관련(추종자)
	_float				m_fWorkGauge;

	static constexpr _float MAX_WORK_GAUGE = 1.f;

public:
	static CBuilding* Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::OBJECTDATA& objData, IMessageChannel* pMessageChannel);

private:
	virtual void Free();
};
