#include "pch.h"
#include "CCookingTargetFood.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCookingStar.h"
#include "CFontMgr.h"


CCookingTargetFood::CCookingTargetFood(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CCookingTargetFood::~CCookingTargetFood()
{
}

HRESULT CCookingTargetFood::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(114 * m_fScale, 93 * m_fScale, 0.2f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	switch (m_eFoodType)
	{
	case FT_NOMAL:
		break;

	case FT_COUNT:
		break;

	case FT_QUALITY:
	{
		CGameObject* pGameObject = nullptr;
		for (int i = 0; i < 3; ++i)
		{
			pGameObject = CCookingStar::Create(m_pGraphicDev, 1, { m_vPos.x - 25.0f + (i * 25.0f), m_vPos.y - 25.0f,0.00001f }, 0.1f);

			if (nullptr == pGameObject)
				return E_FAIL;

			m_vecQualityStar.push_back(pGameObject);
		}
		break;
	}
	case FT_END:
		break;

	default:
		break;
	}


	return S_OK;
}

HRESULT CCookingTargetFood::Ready_Material()
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

_int CCookingTargetFood::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	for (CGameObject* Star : m_vecQualityStar)
	{
		Star->Update_GameObject(fTimeDelta);
	}
	return iExit;
}

void CCookingTargetFood::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
	for (CGameObject* Star : m_vecQualityStar)
	{
		Star->LateUpdate_GameObject(fTimeDelta);
	}
}

void CCookingTargetFood::Render_GameObject()
{
	if (!m_bRender) { return; }

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	if (FOODTYPE::FT_COUNT != m_eFoodType) { return; }
	D3DXCOLOR FontColor = D3DXCOLOR(240.f / 256.f, 240.f / 256.f, 240.f / 256.f, 1.f);
	wchar_t szFoodCount[16];
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	swprintf_s(szFoodCount, L"%d", m_iCraftableCount);
	RECT rc0Player = { 0, 0, m_vPos.x + (WINCX * 0.5) + 30,-m_vPos.y + (WINCY * 0.5f) + 30 };
	CFontMgr::GetInstance()->Render_Font(L"Font_NotoSans30", szFoodCount, rc0Player, FontColor, DT_RIGHT | DT_BOTTOM);


}

void CCookingTargetFood::OnCollision(CGameObject* pObject)
{

}

HRESULT CCookingTargetFood::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingTarget"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


	return S_OK;
}



CCookingTargetFood* CCookingTargetFood::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _float fScale, FOODTYPE eft, _int _iQulity)
{
	CCookingTargetFood* pCookingTarget = new CCookingTargetFood(pGraphicDev);

	pCookingTarget->m_vPos = vPos;
	pCookingTarget->m_fScale = fScale;
	pCookingTarget->m_eFoodType = eft;
	pCookingTarget->m_iQualityStar = _iQulity;


	if (FAILED(pCookingTarget->Ready_GameObject()))
	{
		Safe_Release(pCookingTarget);
		MSG_BOX("pCCookingTargetFood Create Failed");
		return nullptr;
	}

	return pCookingTarget;
}

void CCookingTargetFood::CalcuCraftableCount(_int _ihave, _int _ineed)
{
	m_iCraftableCount = _ihave / _ineed;
}

void CCookingTargetFood::Free()
{
	for (CGameObject* StarUI : m_vecQualityStar)
	{
		Safe_Release(StarUI);
	}
	m_vecQualityStar.clear();
	CUi::Free();
}
