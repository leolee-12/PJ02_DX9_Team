#include "pch.h"
#include "CInvenPlayerHp.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CPersistentMgr.h"

CInvenPlayerHp::CInvenPlayerHp(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CInvenPlayerHp::~CInvenPlayerHp()
{
}

HRESULT CInvenPlayerHp::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(98 * m_fScale, 94 * m_fScale, 1.0f);
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);

	m_iPlayerHp = 0;
	m_iPlayerMaxHp = 0;
	m_bRender = true;
	return S_OK;
}


_int CInvenPlayerHp::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);


	// 일단 업데이트에서 대충넣기

	m_iPlayerHp = CPersistentMgr::GetInstance()->Get_Player()->Get_Hp();
	m_iPlayerMaxHp = CPersistentMgr::GetInstance()->Get_Player()->Get_MaxHp();

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CInvenPlayerHp::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	m_vWorldPos = m_vParentPos + m_vLocalPos;
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vWorldPos);
	Compute_ViewDepth_Ortho(&m_vWorldPos);
}

void CInvenPlayerHp::Render_GameObject()
{
	if (!m_bRender) { return; }

	_int iUICell = m_iPlayerMaxHp / 2;

	for (_int i = 0; i < iUICell; ++i)
	{
		_matrix matTmp = *(m_pTransformCom->Get_World());
		_float fMoveX = 30.f;
		fMoveX *= i;
		matTmp._41 += fMoveX;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matTmp);

		if (m_iPlayerHp > 2 + (i * 2)) {
			m_pTextureCom->Set_Texture(2);
		}
		else {
			_int iCellHP = m_iPlayerHp - (i * 2);
			if (iCellHP < 0)
				iCellHP = 0;

			m_pTextureCom->Set_Texture(iCellHP);
		}

		m_pBufferCom->Render_Buffer();
	}
}

void CInvenPlayerHp::OnCollision(CGameObject* pObject)
{

}

CInvenPlayerHp* CInvenPlayerHp::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale)
{
	CInvenPlayerHp* pCInvenPlayerHp = new CInvenPlayerHp(pGraphicDev);
	pCInvenPlayerHp->m_vParentPos = _vParentPos;
	pCInvenPlayerHp->m_vLocalPos = _vLocalPos;
	pCInvenPlayerHp->m_fScale = _fScale;
	pCInvenPlayerHp->m_vWorldPos = _vParentPos + _vLocalPos;
	if (FAILED(pCInvenPlayerHp->Ready_GameObject()))
	{
		Safe_Release(pCInvenPlayerHp);
		MSG_BOX("pCInvenPlayerHp Create Failed");
		return nullptr;
	}

	return pCInvenPlayerHp;
}

HRESULT CInvenPlayerHp::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlayerHP"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}





void CInvenPlayerHp::Free()
{
	CUi::Free();
}
