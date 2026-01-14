#include "pch.h"
#include "CMainCamera.h"
#include "CDInputMgr.h"
#include "CTransform.h"
#include "CPersistentMgr.h"

CMainCamera::CMainCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev), m_bFix(true), m_bCheck(true), m_fLerpSpeed(0.f), m_bShaking(false), m_fZoom(0.f)
{
	ZeroMemory(&m_vLook, sizeof(_vec3));
	ZeroMemory(&m_vUp, sizeof(_vec3));
	ZeroMemory(&m_vRight, sizeof(_vec3));
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
	: CCamera(rhs), m_bFix(true), m_bCheck(true), m_fLerpSpeed(0.f), m_bShaking(false), m_fZoom(0.f)
{
	ZeroMemory(&m_vLook, sizeof(_vec3));
	ZeroMemory(&m_vUp, sizeof(_vec3));
	ZeroMemory(&m_vRight, sizeof(_vec3));
}

CMainCamera::~CMainCamera()
{
}

void CMainCamera::Set_MainTarget(CTransform* pTargetTransformCom)
{
	if (pTargetTransformCom == nullptr) { return; }

	Safe_Release(m_pTargetTransformCom);
	m_pTargetTransformCom = pTargetTransformCom;
	m_pTargetTransformCom->AddRef();
	return;
}

HRESULT CMainCamera::Ready_GameObject(
	const _float& fFov,
	const _float& fAspect,
	const _float& fNear,
	const _float& fFar)
{
	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	m_fLerpSpeed = 6.f;

	m_fSpeed = 10.f;

	m_eCamState = MCAM_DEFAULT;

	m_pTargetTransformCom = CPersistentMgr::GetInstance()->Get_PlayerTransform();
	m_pTargetTransformCom->AddRef();

	Ready_Event_MainCam();

	_vec3 vTargetPos;
	m_pTargetTransformCom->Get_Info(INFO_POS, &vTargetPos);

	m_vAt = vTargetPos;
	vTargetPos.z -= m_fZoom;
	vTargetPos.y += (m_fZoom * DEFAULTGAPY) / DEFAULTGAPZ;
	m_vEye = vTargetPos;
	m_vUp = { 0.f, 1.f, 0.f };

	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;

	_vec3 vUp = { 0.f, 1.f, 0.f };
	m_vLook = m_vAt - m_vEye;
	D3DXVec3Normalize(&m_vLook, &m_vLook);

	D3DXVec3Cross(&m_vRight, &vUp, &m_vLook);
	D3DXVec3Normalize(&m_vRight, &m_vRight);

	D3DXVec3Cross(&m_vDirUp, &m_vLook, &m_vRight);
	D3DXVec3Normalize(&m_vDirUp, &m_vDirUp);

	m_fZoom = DEFAULTZOOM;


	return S_OK;
}

_int CMainCamera::Update_GameObject(const _float& fTimeDelta)
{
	switch (m_eCamState) 
	{
	case MCAM_DEFAULT:
		Default_CameraSetting(fTimeDelta);
		Shaking_CameraSetting(fTimeDelta);
		break;
	case MCAM_STAGING:
		Staging_CameraSetting(fTimeDelta);
	}


	_int		iExit = CCamera::Update_GameObject(fTimeDelta);

	return iExit;
}

void CMainCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CCamera::LateUpdate_GameObject(fTimeDelta);
}

void CMainCamera::Default_CameraSetting(const _float& fTimeDelta)
{
	//if (m_bShaking == true) { return; }

	_vec3 vTargetPos;
	m_pTargetTransformCom->Get_Info(INFO_POS, &vTargetPos);
	D3DXVec3Lerp(&m_vAt, &m_vAt, &vTargetPos, fTimeDelta * m_fLerpSpeed);
	vTargetPos.z -= m_fZoom;
	vTargetPos.y += (m_fZoom * DEFAULTGAPY) / DEFAULTGAPZ;
	D3DXVec3Lerp(&m_vEye, &m_vEye, &vTargetPos, fTimeDelta * m_fLerpSpeed);
}

void CMainCamera::Shaking_CameraSetting(const _float& fTimeDelta)
{
	if (m_bShaking == false) { return; }

	if (m_fShakeTime > 0.f)
	{
		m_fShakeTime -= fTimeDelta;
		m_fElapsedTime += fTimeDelta;

		Vec2 vOffset = Engine::Get_Offset(m_fElapsedTime * m_fShakeTempo);
		_float fOffsetX = vOffset.x * m_fShakeStrength;
		_float fOffsetY = vOffset.y * m_fShakeStrength;

		_vec3 vShake = (m_vRight * fOffsetX) + (m_vDirUp * fOffsetY);

		m_vEye = m_vOriginPos + vShake;
		m_vAt = m_vOriginAt + vShake;

		/*m_vEye = m_vOriginPos + _vec3(fOffsetX, fOffsetY, 0.f);
		m_vAt = m_vOriginAt + _vec3(fOffsetX, fOffsetY, 0.f);*/
	}
	else
	{
		m_vEye = m_vOriginPos;
		m_vAt = m_vOriginAt;
		m_bShaking = false;
	}
}

void CMainCamera::Staging_CameraSetting(const _float& fTimeDelta)
{
	_vec3 vTargetPos = m_vAtStatic;
	D3DXVec3Lerp(&m_vAt, &m_vAt, &vTargetPos, fTimeDelta * m_fLerpSpeed);
	vTargetPos.z -= m_fZoom;
	vTargetPos.y += (m_fZoom * DEFAULTGAPY) / DEFAULTGAPZ;
	D3DXVec3Lerp(&m_vEye, &m_vEye, &vTargetPos, fTimeDelta * m_fLerpSpeed);
}

void CMainCamera::Ready_Event_MainCam()
{
	m_hmapSubHandles.insert({ L"Player_Attacked", m_pMessageChannel->Subscribe(L"Monster.Attacked", [this](const IMessageChannel::EVENT& Event) {
		Set_Shake(0.3f, 0.2f, 20.f);
	}) });

	m_hmapSubHandles.insert({ L"Set_CutSceneTarget", m_pMessageChannel->Subscribe(L"CutScene.CameraTarget", [this](const IMessageChannel::EVENT& Event) {
		auto TargetPositer = Event.hmapData.find(L"TargetPos");
		if (TargetPositer == Event.hmapData.end()) { return; }
		auto Zoomiter = Event.hmapData.find(L"Zoom");
		if (Zoomiter == Event.hmapData.end()) { return; }


		Set_CutScene_LookAt(any_cast<_vec3>(TargetPositer->second));
		Reset_Zoom();
		Set_Zoom(any_cast<_float>(Zoomiter->second));
		m_eCamState = MCAM_STAGING;
	}) });

	m_hmapSubHandles.insert({ L"End_CutScene", m_pMessageChannel->Subscribe(L"CutScene.End", [this](const IMessageChannel::EVENT& Event) {
		Reset_Zoom();
		m_eCamState = MCAM_DEFAULT;
	}) });
}

void CMainCamera::Set_Shake(_float fStrength, _float fTime, _float fTempo)
{
	m_bShaking = true;
	m_fShakeStrength = fStrength;
	m_fShakeTime = fTime;
	m_fElapsedTime = 0.f;
	m_fShakeTempo = fTempo;
	m_vOriginPos = m_vEye;
	m_vOriginAt = m_vAt;
}

void CMainCamera::Set_Zoom(_float fZoom)
{
	m_fZoom /= fZoom;
}

void CMainCamera::Set_CutScene_LookAt(_vec3 vAt)
{
	m_vAtStatic = vAt;
}

void CMainCamera::Reset_Zoom()
{
	m_fZoom = DEFAULTZOOM;
}


CMainCamera* CMainCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	IMessageChannel* pMessageChannel,
	const _float& fFov, const _float& fAspect,
	const _float& fNear, const _float& fFar)
{
	CMainCamera* pCamera = new CMainCamera(pGraphicDev);

	if (pMessageChannel != nullptr) {
		pCamera->m_pMessageChannel = pMessageChannel;
		pCamera->m_pMessageChannel->AddRef();
	}

	if (FAILED(pCamera->Ready_GameObject(fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pCamera);
		MSG_BOX("CMainCamera Create Failed");
		return nullptr;
	}

	return pCamera;
}

void CMainCamera::Free()
{
	Safe_Release(m_pTargetTransformCom);
	CCamera::Free();
}
