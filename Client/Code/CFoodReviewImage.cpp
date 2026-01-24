#include "pch.h"
#include "CFoodReviewImage.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CFoodReviewImage::CFoodReviewImage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CFoodReviewImage::~CFoodReviewImage()
{
}

HRESULT CFoodReviewImage::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(378.f * m_fScale, 261.f * m_fScale, 0.1f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	return S_OK;
}

_int CFoodReviewImage::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CFoodReviewImage::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CFoodReviewImage::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(m_iPage);

	m_pBufferCom->Render_Buffer();
}

void CFoodReviewImage::OnCollision(CGameObject* pObject)
{

}

HRESULT CFoodReviewImage::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ReviewImage"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


CFoodReviewImage* CFoodReviewImage::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale, _int _iPage)
{
	CFoodReviewImage* pCFoodReviewImage = new CFoodReviewImage(pGraphicDev);

	pCFoodReviewImage->m_vPos = _vPos;
	pCFoodReviewImage->m_fScale = _fScale;
	pCFoodReviewImage->m_iPage = _iPage;

	if (FAILED(pCFoodReviewImage->Ready_GameObject()))
	{
		Safe_Release(pCFoodReviewImage);
		MSG_BOX("pCFoodReviewImage Create Failed");
		return nullptr;
	}
	return pCFoodReviewImage;
}

void CFoodReviewImage::Free()
{
	CUi::Free();
}
