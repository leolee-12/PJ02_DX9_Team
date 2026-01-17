#include "pch.h"
#include "CSelectionArrow.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CSelectionArrow::CSelectionArrow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CSelectionArrow::~CSelectionArrow()
{
}

HRESULT CSelectionArrow::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(214.f * 0.5f, 78.f * 0.5f, 0.f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	return S_OK;
}

HRESULT CSelectionArrow::Ready_Material()
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

void CSelectionArrow::Turn()
{
	switch (m_iDir)
	{
	case 0:
		if (m_fCurAngle >= 0)
		{
			m_fCurAngle = 0.f;
			return;
		}
		m_pTransformCom->Rotation(ROT_Z, 10.f);
		m_fCurAngle += 10.f;
		break;
	case 1:
		if (m_fCurAngle <= -180.f)
		{
			m_fCurAngle = -180.f;
			return;
		}
		m_pTransformCom->Rotation(ROT_Z, -10.f);
		m_fCurAngle -= 10.f;
		break;
	default:
		break;
	}
}

_int CSelectionArrow::Update_GameObject(const _float& fTimeDelta)
{
	Turn();
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if (m_bActive) {
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CSelectionArrow::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CSelectionArrow::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());


	m_pTextureCom->Set_Texture();


	m_pBufferCom->Render_Buffer();
}

void CSelectionArrow::OnCollision(CGameObject* pObject)
{

}

HRESULT CSelectionArrow::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SelectionArrow"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}


CSelectionArrow* CSelectionArrow::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos)
{
	CSelectionArrow* pSelectionArrow = new CSelectionArrow(pGraphicDev);

	pSelectionArrow->m_vPos = _vPos;


	if (FAILED(pSelectionArrow->Ready_GameObject()))
	{
		Safe_Release(pSelectionArrow);
		MSG_BOX("pSelectionArrow Create Failed");
		return nullptr;
	}
	return pSelectionArrow;
}

void CSelectionArrow::Free()
{
	CUi::Free();
}
