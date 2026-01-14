#include "pch.h"
#include "CTerrainWall.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CTerrainWall::CTerrainWall(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_optPos(nullopt), m_optRotY(nullopt)
{
}

CTerrainWall::CTerrainWall(LPDIRECT3DDEVICE9 pGraphicDev, optional<_vec3> vPos, optional<_float> fRotY)
	: CGameObject(pGraphicDev), m_optPos(vPos), m_optRotY(fRotY)
{

}

CTerrainWall::CTerrainWall(const CTerrainWall& rhs)
	: CGameObject(rhs), m_optPos(rhs.m_optPos), m_optRotY(rhs.m_optRotY)
{
}

CTerrainWall::~CTerrainWall()
{
}

HRESULT CTerrainWall::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (m_optPos.has_value()) {
		_vec3 vMovePos = m_optPos.value();
		m_pTransformCom->Set_Pos(vMovePos.x, vMovePos.y, vMovePos.z);
	}

	if (m_optRotY.has_value()) {
		_float fRotY = m_optRotY.value();
		m_pTransformCom->Rotation(ROT_Y, fRotY);
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	return S_OK;
}

_int CTerrainWall::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

void CTerrainWall::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTerrainWall::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);


	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

}

HRESULT CTerrainWall::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// TerrainTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CTerrainWallTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TerrainWallTex"));

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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TerrainWallTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


	return S_OK;
}

CTerrainWall* CTerrainWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, optional<_vec3> vPos, optional<_float> fRotY)
{
	CTerrainWall* pTerrainWall = new CTerrainWall(pGraphicDev, vPos, fRotY);

	if (FAILED(pTerrainWall->Ready_GameObject()))
	{
		Safe_Release(pTerrainWall);
		MSG_BOX("pTerrainWall Create Failed");
		return nullptr;
	}

	return pTerrainWall;
}

void CTerrainWall::Free()
{
	Safe_Release(m_pBufferCom);

	CGameObject::Free();
}
