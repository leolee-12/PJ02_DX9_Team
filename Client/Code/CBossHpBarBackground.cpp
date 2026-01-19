#include "pch.h"
#include "CBossHpBarBackground.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CBossHpBarBackground::CBossHpBarBackground(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CBossHpBarBackground::~CBossHpBarBackground()
{
}

HRESULT CBossHpBarBackground::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale((61.f * 8.f) - 10.f, 9.f * 3.f, 0.1f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	return S_OK;
}

_int CBossHpBarBackground::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CBossHpBarBackground::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CBossHpBarBackground::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pTextureCom->Set_Texture();


	m_pBufferCom->Render_Buffer();
}

void CBossHpBarBackground::OnCollision(CGameObject* pObject)
{

}

HRESULT CBossHpBarBackground::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BossHpBarCover"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


CBossHpBarBackground* CBossHpBarBackground::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos)
{
	CBossHpBarBackground* pBossHpBarBackground = new CBossHpBarBackground(pGraphicDev);

	pBossHpBarBackground->m_vPos = _vPos;
	//pBossHpBarBackground->m_fScale = _fScale;


	if (FAILED(pBossHpBarBackground->Ready_GameObject()))
	{
		Safe_Release(pBossHpBarBackground);
		MSG_BOX("pBossHpBarBackground Create Failed");
		return nullptr;
	}
	return pBossHpBarBackground;
}

void CBossHpBarBackground::Free()
{
	CUi::Free();
}
