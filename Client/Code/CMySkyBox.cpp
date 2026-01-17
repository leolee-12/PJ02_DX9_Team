#include "pch.h"
#include "CMySkyBox.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

CMySkyBox::CMySkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_dwColor(0xFFF8EFD6)
{
}

CMySkyBox::CMySkyBox(const CGameObject& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_dwColor(0xFFF8EFD6)
{
}

CMySkyBox::~CMySkyBox()
{
}

HRESULT CMySkyBox::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(40.f, 40.f, 40.f);

	return S_OK;
}

_int CMySkyBox::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

void CMySkyBox::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	// 카메라 위치 따라가기
	_matrix matCamWorld;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
	D3DXMatrixInverse(&matCamWorld, 0, &matCamWorld);

	m_pTransformCom->Set_Pos(matCamWorld._41, matCamWorld._42 + 3.f, matCamWorld._43);
}

void CMySkyBox::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// 단색 렌더링 (텍스처 없음)
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, m_dwColor);
	//m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFBFBFBF);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

	m_pBufferCom->Render_Buffer();

	// 렌더 스테이트 복원
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CMySkyBox::Set_SkyType(_int iSkyType)
{
	// 0 = Day (#F8EFD6), 1 = Night (검은색)
	m_dwColor = (iSkyType == 0) ? 0xFFF8EFD6 : 0xFF000000;
}

HRESULT CMySkyBox::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex 버퍼
	pComponent = m_pBufferCom = dynamic_cast<Engine::CCubeTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// 트랜스폼
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}

CMySkyBox* CMySkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iSkyType)
{
	CMySkyBox* pSkyBox = new CMySkyBox(pGraphicDev);

	if (FAILED(pSkyBox->Ready_GameObject()))
	{
		Safe_Release(pSkyBox);
		MSG_BOX("CMySkyBox Create Failed");
		return nullptr;
	}

	pSkyBox->Set_SkyType(iSkyType);

	return pSkyBox;
}

void CMySkyBox::Free()
{
	// m_pBufferCom, m_pTransformCom은 m_mapComponent에 등록되어 있음
	// CGameObject::Free()에서 자동 해제됨
	CGameObject::Free();
}
