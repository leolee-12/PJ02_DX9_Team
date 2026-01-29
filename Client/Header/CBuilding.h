#pragma once
#include "CGameObject.h"
#include "IInteractable.h"
#include "Building_Enum.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CTriggerPoint;
class CResourceWorkBar;

class CBuilding : public CGameObject, public IInteractable
{
public:
	//enum BUILDING_TYPE { BT_DUMMY, BT_WORKSHOP, BT_COOK, BT_KNUCKLEBONE, BT_SHRINE, BT_END };
	enum BUILDING_STATE { BS_PREVIEW, BS_CONSTRUCTING, BS_COMPLETE, BS_END };

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
	virtual _vec3*		Get_WorkPos(_vec3* pWorkPos) const;

	_bool				Get_CanPlace() { return m_bCanPlace; }

	wstring				Get_CompleteTexKey();

	void				Set_PosForPick(const _vec3& vPos);

	void				Set_Placement();

	virtual void		PrepareDestroy();

private:
	HRESULT	Add_Component();
	void	Change_State(BUILDING_STATE eState);
	void	Player_Interact();
	void	Set_Texture();
	void	Ready_Variable();
	void	Ready_Event();
	void	Ready_Trigger();
	void	Update_WorkBar(const _float& fTimeDelta);

	HRESULT 		Ready_PixelShader();
private:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCollider*	m_pColliderCom;
	CTriggerPoint*		m_pTrigger;


	BUILDING_TYPE	m_eBuildingType;
	BUILDING_STATE	m_eBuildingState;
	_float			m_fGroundY;
	_float			m_fAccTime;

	_vec3			m_vPos = {};

	// 트리거 관련
	_bool			m_bUsingTrigger = true;

	// 상호작용 관련(추종자)
	_float					m_fPreWorkGauge;
	_float					m_fWorkGauge;
	_uint					m_iTexIdx;
	CResourceWorkBar*		m_pWorkBar;

	// 프리뷰 상태 관련
	_bool					m_bCanPlace;
	LPDIRECT3DPIXELSHADER9 m_pPixelShader;

	static constexpr _float MAX_WORK_GAUGE = 1.f;
	static constexpr _float DEFAULT_CONSTRUCT_GROUNDY = -2.49f;
	static constexpr _float DEFAULT_COMPLETE_GROUNDY = -0.5f;
	vector<CGameObject*>	m_vecSubObjects;

public:
	static CBuilding* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, const _vec3& vPos, BUILDING_TYPE eType, BUILDING_STATE eState = BS_CONSTRUCTING);

private:
	virtual void Free();
};
