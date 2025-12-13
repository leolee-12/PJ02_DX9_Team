#pragma once

#include "CBase.h"
#include "CGraphicDev.h"
#include "CTimerMgr.h"
#include "CFrameMgr.h"
#include "CManagement.h"

class CMainApp : public CBase
{
	// 생성자, 소멸자

private:
	explicit CMainApp();
	virtual ~CMainApp();

public:
	HRESULT			Ready_MainApp();
	int				Update_MainApp(const float& fTimeDelta);
	void			LateUpdate_MainApp(const float& fTimeDelta);
	void			Render_MainApp();

private:
	HRESULT		Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	Engine::CGraphicDev*	m_pDeviceClass;
	Engine::CManagement* m_pManagementClass;

public:
	static CMainApp* Create();

private:
	virtual void		Free();

};

// 1. viewspace 변환 행렬과 원근 투영 행렬 함수 직접 구현하기



// 1. 평면의 방정식을 이용하여 지형 타기를 만들기		(ax + by + cz + d)
// D3DXPLANE		Plane;
// D3DXPlaneFromPoints();


// 2. 마우스 피킹을 이용하여 지형 타기를 만들기
// D3DXIntersectTri();

// 지형 vtx 어느 영역에 있는가?	- local, world
// 마우스는 어느 영역에 있는가? - viewport(window)