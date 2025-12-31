#include "pch.h"
#include "CKBBoardSlot.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

using namespace KBBoard;

CKBBoardSlot::CKBBoardSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_iOwner(0)
	, m_iCol(0)
	, m_iRow(0)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CKBBoardSlot::~CKBBoardSlot()
{
}

HRESULT CKBBoardSlot::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	// 슬롯 위치 계산
	// 열: 0=왼쪽, 1=중앙, 2=오른쪽
	_float fX = BOARD_CENTER_X + (m_iCol - 1) * SLOT_STEP_X;

	// 행: 0=중간선에 가까움, 2=멀리
	// NPC: 행 0이 아래(중간선), 행 2가 위
	// 플레이어: 행 0이 위(중간선), 행 2가 아래
	_float fY = 0.f;
	if (m_iOwner == 1)	// NPC
	{
		fY = NPC_BOARD_Y + (m_iRow) * SLOT_STEP_Y;
	}
	else				// 플레이어
	{
		fY = PLAYER_BOARD_Y - (m_iRow) * SLOT_STEP_Y;
	}

	m_vPos = _vec3(fX, fY, 0.1f);

	m_pTransformCom->Set_Scale(SLOT_RENDER_WIDTH, SLOT_RENDER_HEIGHT, 1.f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);

	return S_OK;
}

_int CKBBoardSlot::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CKBBoardSlot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CKBBoardSlot::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	D3DMATERIAL9 tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Emissive = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CKBBoardSlot::OnCollision(CGameObject* pObject)
{
}

HRESULT CKBBoardSlot::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTex
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

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_KBBoardSlot"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CKBBoardSlot* CKBBoardSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iOwner, _int iCol, _int iRow)
{
	CKBBoardSlot* pSlot = new CKBBoardSlot(pGraphicDev);
	pSlot->m_iOwner = iOwner;
	pSlot->m_iCol = iCol;
	pSlot->m_iRow = iRow;

	if (FAILED(pSlot->Ready_GameObject()))
	{
		Safe_Release(pSlot);
		MSG_BOX("CKBBoardSlot Create Failed");
		return nullptr;
	}

	return pSlot;
}

void CKBBoardSlot::Free()
{
	CUi::Free();
}
