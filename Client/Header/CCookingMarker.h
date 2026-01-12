#pragma once
#include "CUi.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}


enum CookingMarkerState
{
	MS_MOVELEFT,
	MS_MOVERIGHT,
	MS_STOP,
	MS_END,
};

class CCookingMarker :
	public CUi
{
private:
	explicit CCookingMarker(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCookingMarker();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CCookingMarker* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	void Set_MarkerState(CookingMarkerState eState) { m_eCurMarkerState = eState; }
	void Set_MarkerPos(_float fPos) { m_vPos.x = fPos; }
	void Move_Marker(const _float fTimeDelta);
	void Stop_Marker();
	float Get_MarkerPos();
	CookingMarkerState Get_MarkerState() { return m_eCurMarkerState; }
private:
	virtual			void		Free();

	HRESULT						Add_Component();
	HRESULT						Ready_Material();

private:
	CRcTex* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture* m_pTextureCom;

	CookingMarkerState m_ePreMarkerState;
	CookingMarkerState m_eCurMarkerState;


	// 마커 이동 속도 사실그냥 Gauge 끝값
	float m_fMoveSpeed;

	// -1 ~ 1 사이값 
	float m_fMarkerOffset;

	// 좌우 이동 범위   게이지 * 0.5
	float fMoveRange;

	// 마커 Stop 관련 멤버변수
	float m_fStopTime;
	float m_fCurStopTime;
};

