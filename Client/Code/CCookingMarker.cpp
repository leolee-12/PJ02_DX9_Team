#include "pch.h"
#include "CCookingMarker.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CCookingMarker::CCookingMarker(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_eCurMarkerState(MS_END)
	, m_ePreMarkerState(MS_END)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CCookingMarker::~CCookingMarker()
{
}

HRESULT CCookingMarker::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(36.f * 0.4f, 104.f * 0.4f, 0.f);

	m_fMarkerOffset = 0.f;
	m_fMoveSpeed = 2.0f;
	m_fStopTime = 0.5f;
	m_fCurStopTime = 0.f;
	m_eCurMarkerState = MS_MOVELEFT;
	m_ePreMarkerState = MS_END;
	fMoveRange = 598 * 0.4 * 0.5;
	return S_OK;
}

_int CCookingMarker::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CCookingMarker::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	Move_Marker(fTimeDelta);

	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CCookingMarker::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Set_Texture();
	m_pBufferCom->Render_Buffer();
}

void CCookingMarker::Move_Marker(const _float fTimeDelta)
{
	switch (m_eCurMarkerState)
	{
	case CookingMarkerState::MS_MOVELEFT:
		m_fMarkerOffset -= m_fMoveSpeed * fTimeDelta;
		if (m_fMarkerOffset <= -1.f)
		{
			m_fMarkerOffset = -1.f;
			m_eCurMarkerState = MS_MOVERIGHT;
		}
		break;

	case CookingMarkerState::MS_MOVERIGHT:
		m_fMarkerOffset += m_fMoveSpeed * fTimeDelta;
		if (m_fMarkerOffset >= 1.f)
		{
			m_fMarkerOffset = 1.f;
			m_eCurMarkerState = MS_MOVELEFT;
		}
		break;

	case CookingMarkerState::MS_STOP:
		m_fCurStopTime += fTimeDelta;
		if (m_fCurStopTime >= m_fStopTime)
		{
			m_eCurMarkerState = m_ePreMarkerState;
		}
		break;
	}
	float fX = m_fMarkerOffset * fMoveRange;

	m_pTransformCom->Set_Pos(fX, WINCY / 5, 0.1f);
}

void CCookingMarker::Stop_Marker()
{
	if (m_eCurMarkerState == MS_STOP)
		return;


	m_fCurStopTime = 0.0f;
	m_ePreMarkerState = m_eCurMarkerState;
	m_eCurMarkerState = MS_STOP;
}

float CCookingMarker::Get_MarkerPos()
{ 
	_vec3 temp; 
	m_pTransformCom->Get_Info(INFO_POS, &temp); 
	return temp.x; 
};

HRESULT CCookingMarker::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(
		Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
	if (!pComponent) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
		Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	if (!pComponent) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(
		Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingMarker"));
	if (!pComponent) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CCookingMarker::OnCollision(CGameObject* pObject)
{

}

CCookingMarker* CCookingMarker::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCookingMarker* pObj = new CCookingMarker(pGraphicDev);

	if (FAILED(pObj->Ready_GameObject()))
	{
		Safe_Release(pObj);
		MSG_BOX("CCookingMarker Create Failed");
		return nullptr;
	}
	return pObj;
}

void CCookingMarker::Free()
{
	CUi::Free();
}