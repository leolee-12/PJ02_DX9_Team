#include "pch.h"
#include "CBossHpBarFront.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CBossHpBarFront::CBossHpBarFront(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CBossHpBarFront::~CBossHpBarFront()
{
}

HRESULT CBossHpBarFront::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(59 * 2*m_fScale, 7 * m_fScale, 0.1f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	m_fRadio = 1.0f;

	return S_OK;
}

HRESULT CBossHpBarFront::Ready_Material()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

_int CBossHpBarFront::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CBossHpBarFront::LateUpdate_GameObject(const _float& fTimeDelta)
{
	// 임시테스트용 셰이더 넣고 삭제 요망
	m_pTransformCom->Set_Scale(59 * 2 * m_fScale*m_fRadio, 7 * m_fScale, 0.1f);


	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CBossHpBarFront::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pTextureCom->Set_Texture();


	m_pBufferCom->Render_Buffer();
}

void CBossHpBarFront::OnCollision(CGameObject* pObject)
{

}

HRESULT CBossHpBarFront::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BossHpBarFront"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


CBossHpBarFront* CBossHpBarFront::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, float _fScale)
{
	CBossHpBarFront* pBossHpBarFront = new CBossHpBarFront(pGraphicDev);

	pBossHpBarFront->m_vPos = _vPos;
	pBossHpBarFront->m_fScale = _fScale;


	if (FAILED(pBossHpBarFront->Ready_GameObject()))
	{
		Safe_Release(pBossHpBarFront);
		MSG_BOX("pBossHpBarFront Create Failed");
		return nullptr;
	}
	return pBossHpBarFront;
}

void CBossHpBarFront::Free()
{
	CUi::Free();
}
