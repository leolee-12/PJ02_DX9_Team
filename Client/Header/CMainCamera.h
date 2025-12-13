#pragma once

#include "CCamera.h"
#include "Engine_Define.h"

namespace Engine
{
	class CTransform;
}

class CMainCamera : public CCamera
{
private:
	explicit CMainCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMainCamera(const CMainCamera& rhs);
	virtual ~CMainCamera();

public:
	HRESULT		Ready_GameObject(const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov,
		const _float& fAspect,
		const _float& fNear,
		const _float& fFar);

	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);


private:
	_float		m_fSpeed;
	_bool		m_bFix;
	_bool		m_bCheck;
	Engine::CTransform* m_pTargetTransformCom;

public:
	//void		Set_Target();

private:
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move();
	void		Mouse_Fix();

public:
	static CMainCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov = D3DXToRadian(60.f),
		const _float& fAspect = (_float)WINCX / WINCY,
		const _float& fNear = 0.1f,
		const _float& fFar = 1000.f);

private:
	virtual void Free();


};

