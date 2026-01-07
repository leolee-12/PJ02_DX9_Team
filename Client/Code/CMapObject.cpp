#include "pch.h"
#include "CMapObject.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"

CMapObject::CMapObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_iTextureIndex(0)
	, m_fScale(1.f)
	, m_ePlacement(PLACEMENT_STANDING)
{
}

CMapObject::CMapObject(const CMapObject& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_strCategory(rhs.m_strCategory)
	, m_iTextureIndex(rhs.m_iTextureIndex)
	, m_fScale(rhs.m_fScale)
	, m_ePlacement(rhs.m_ePlacement)
{
}

CMapObject::~CMapObject()
{
}

HRESULT CMapObject::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CMapObject::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CMapObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(Engine::INFO_POS, &vPos);
	Compute_ViewDepth(&vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMapObject::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 알파 테스트 (완전 투명 픽셀 제거)
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x10);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Set_Texture(m_iTextureIndex);
	m_pBufferCom->Render_Buffer();

	// 설정 복원
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CMapObject::Set_ObjectData(const Engine::OBJECTDATA& objData)
{
	m_strCategory = objData.category;
	m_iTextureIndex = objData.textureIndex;
	m_fScale = objData.scale;
	m_ePlacement = static_cast<PLACEMENT_TYPE>(objData.placement);

	// Set position
	m_pTransformCom->Set_Pos(objData.x, objData.y, objData.z);

	// Set scale
	m_pTransformCom->Set_Scale(m_fScale, m_fScale, m_fScale);

	// If floor placement, rotate to lie flat (앞면이 위를 향하도록)
	if (PLACEMENT_FLOOR == m_ePlacement)
	{
		m_pTransformCom->Rotation(ROT_X, 90.f);
	}
}

HRESULT CMapObject::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTex Buffer
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

	// 카테고리에 따라 텍스처 로드
	const _tchar* szTextureProto = L"Proto_RockTexture";  // 기본값

	if (m_strCategory == "Rock")
		szTextureProto = L"Proto_RockTexture";
	else if (m_strCategory == "Tree")
		szTextureProto = L"Proto_TreeTexture";
	else if (m_strCategory == "Fire")
		szTextureProto = L"Proto_FireTexture";
	else if (m_strCategory == "Tent")
		szTextureProto = L"Proto_TentTexture";
	else if (m_strCategory == "Twig")
		szTextureProto = L"Proto_TreeTexture";  // Twig는 Tree 텍스처 사용

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(szTextureProto));

	if (nullptr == pComponent)
	{
		// 폴백: 특정 텍스처 없으면 Rock 텍스처 사용
		pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
			(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RockTexture"));
	}

	if (nullptr != pComponent)
		m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CMapObject* CMapObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::OBJECTDATA& objData)
{
	CMapObject* pObject = new CMapObject(pGraphicDev);

	// Add_Component()에서 올바른 텍스처를 로드할 수 있도록 먼저 카테고리 설정
	pObject->m_strCategory = objData.category;

	if (FAILED(pObject->Ready_GameObject()))
	{
		Safe_Release(pObject);
		MSG_BOX("CMapObject Create Failed");
		return nullptr;
	}

	pObject->Set_ObjectData(objData);

	return pObject;
}

void CMapObject::Free()
{
	// m_pBufferCom, m_pTransformCom, m_pTextureCom are in m_mapComponent
	// They will be released by CGameObject::Free()
	CGameObject::Free();
}
