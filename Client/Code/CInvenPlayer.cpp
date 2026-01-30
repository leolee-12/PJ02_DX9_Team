#include "pch.h"
#include "CInvenPlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CInvenPlayer::CInvenPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CInvenPlayer::~CInvenPlayer()
{
}

HRESULT CInvenPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	m_pTransformCom->Set_Scale(512.0f*m_fScale, 512.0f*m_fScale, 1.0f);
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);

	m_fFrame = 0.0f;
	m_fFrameEnd = 72.0f;
	m_fFrameSpeed = 24.0f;
	m_bRender = true;
	return S_OK;
}


_int CInvenPlayer::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return NOEVENT; }

	Move_Frame(fTimeDelta);
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CInvenPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	m_vWorldPos = m_vParentPos + m_vLocalPos;
	m_pTransformCom->Set_Pos(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vWorldPos);
	Compute_ViewDepth_Ortho(&m_vWorldPos);
}

void CInvenPlayer::Render_GameObject()
{
	if (!m_bRender) { return; }
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture(UINT(m_fFrame));

	m_pBufferCom->Render_Buffer();
}

void CInvenPlayer::OnCollision(CGameObject* pObject)
{

}

CInvenPlayer* CInvenPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vLocalPos, _vec3 _vParentPos, _float _fScale)
{
	CInvenPlayer* pCInvenPlayer = new CInvenPlayer(pGraphicDev);
	pCInvenPlayer->m_vParentPos = _vParentPos;
	pCInvenPlayer->m_vLocalPos = _vLocalPos;
	pCInvenPlayer->m_fScale = _fScale;
	pCInvenPlayer->m_vWorldPos = _vParentPos + _vLocalPos;
	if (FAILED(pCInvenPlayer->Ready_GameObject()))
	{
		Safe_Release(pCInvenPlayer);
		MSG_BOX("pCInvenPlayer Create Failed");
		return nullptr;
	}

	return pCInvenPlayer;
}

HRESULT CInvenPlayer::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InventoryPlayer"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CInvenPlayer::Move_Frame(const _float& fTimeDelta)
{
	m_fFrame += m_fFrameSpeed * fTimeDelta;
	if (m_fFrame > m_fFrameEnd)
	{
		m_fFrame = 0.f;
	}

}



void CInvenPlayer::Free()
{
	CUi::Free();
}
