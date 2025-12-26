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
	void	Set_MainTarget(CTransform* pTargetTransformCom);

public:
	HRESULT		Ready_GameObject(
		const _float& fFov,
		const _float& fAspect,
		const _float& fNear,
		const _float& fFar);

	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);

private:
	void		Default_CameraSetting(const _float& fTimeDelta);
	void		Shaking_CameraSetting(const _float& fTimeDelta);
	void		Ready_Event_MainCam();
	void		Set_Shake(_float fStrength, _float fTime, _float fTempo);


private:
	_float		m_fSpeed;
	_bool		m_bFix;
	_bool		m_bCheck;
	_float		m_fLerpSpeed;

	// 카메라 셰이킹 변수
	_bool		m_bShaking;
	_float		m_fShakeStrength;
	_float		m_fShakeTime;
	_float		m_fElapsedTime;
	_vec3		m_vOriginPos;
	_vec3		m_vOriginAt;
	_float		m_fShakeTempo;

	// 카메라기준 방향벡터
	_vec3		m_vRight;
	_vec3		m_vDirUp;
	_vec3		m_vLook;

	Engine::CTransform* m_pTargetTransformCom;


public:
	static CMainCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		IMessageChannel* pMessageChannel,
		const _float& fFov = D3DXToRadian(60.f),
		const _float& fAspect = (_float)WINCX / WINCY,
		const _float& fNear = 0.1f,
		const _float& fFar = 1000.f);

private:
	virtual void Free();


};

