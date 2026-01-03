#include "pch.h"
#include "CTile.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CMapLoader.h"

CTile::CTile(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_iGridX(0)
	, m_iGridZ(0)
	, m_iTextureId(0)
	, m_iMaskFlags(0)
{
	ZeroMemory(m_pMaskTextures, sizeof(m_pMaskTextures));
}

CTile::CTile(const CTile& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_iGridX(rhs.m_iGridX)
	, m_iGridZ(rhs.m_iGridZ)
	, m_iTextureId(rhs.m_iTextureId)
	, m_iMaskFlags(rhs.m_iMaskFlags)
{
	ZeroMemory(m_pMaskTextures, sizeof(m_pMaskTextures));
}

CTile::~CTile()
{
}

HRESULT CTile::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CTile::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

void CTile::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTile::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	// Render base tile
	m_pTextureCom->Set_Texture(m_iTextureId);
	m_pBufferCom->Render_Buffer();

	// Render masks with alpha blending
	if (m_iMaskFlags != Engine::MASK_NONE)
	{
		//Render_Masks();
	}
}

void CTile::Set_TileData(_int iGridX, _int iGridZ, _int iTextureId, _int iMaskFlags)
{
	m_iGridX = iGridX;
	m_iGridZ = iGridZ;
	m_iTextureId = iTextureId;
	m_iMaskFlags = iMaskFlags;

	// Calculate tile corner position
	_float fTileSize = Engine::CMapLoader::TILE_SIZE;
	_float fPosX = static_cast<_float>(iGridX) * fTileSize;
	_float fPosZ = static_cast<_float>(iGridZ) * fTileSize;

	// Convert to tile center position (RcTex renders from center)
	_float fCenterX = fPosX + fTileSize * 0.5f;
	_float fCenterZ = fPosZ + fTileSize * 0.5f;

	m_pTransformCom->Set_Pos(fCenterX, 0.f, fCenterZ);
	m_pTransformCom->Set_Scale(fTileSize, 1.f, fTileSize);  // X, Z scale (XZ plane buffer)
}

void CTile::Render_Masks()
{
	// Enable alpha blending (standard settings)
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Mask color = black (use texture alpha only)
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF000000);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	_matrix matWorld;
	_float fTileSize = Engine::CMapLoader::TILE_SIZE;
	_float fPosX = static_cast<_float>(m_iGridX) * fTileSize;
	_float fPosZ = static_cast<_float>(m_iGridZ) * fTileSize;

	// Y offset to prevent Z-fighting
	const _float fYOffsets[4] = { 0.02f, 0.021f, 0.022f, 0.023f };
	const _int iMaskBits[4] = { Engine::MASK_DOWN, Engine::MASK_LEFT, Engine::MASK_RIGHT, Engine::MASK_UP };

	for (_int i = 0; i < 4; ++i)
	{
		if ((m_iMaskFlags & iMaskBits[i]) && m_pMaskTextures[i])
		{
			D3DXMatrixIdentity(&matWorld);

			_matrix matScale, matTrans;
			D3DXMatrixScaling(&matScale, fTileSize, 1.f, fTileSize);  // XZ scale
			D3DXMatrixTranslation(&matTrans, fPosX + fTileSize * 0.5f, fYOffsets[i], fPosZ + fTileSize * 0.5f);

			matWorld = matScale * matTrans;
			m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

			m_pMaskTextures[i]->Set_Texture(0);
			m_pBufferCom->Render_Buffer();
		}
	}

	// Restore TextureStage settings
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

HRESULT CTile::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTexXZ Buffer (XZ plane - no rotation needed)
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTexXZ*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTexXZ"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Tile Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TileTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Mask Textures (Down, Left, Right, Up)
	const _tchar* szMaskProtos[4] = {
		L"Proto_TileMask_Down",
		L"Proto_TileMask_Left",
		L"Proto_TileMask_Right",
		L"Proto_TileMask_Up"
	};

	for (_int i = 0; i < 4; ++i)
	{
		m_pMaskTextures[i] = dynamic_cast<Engine::CTexture*>
			(Engine::CProtoMgr::GetInstance()->Clone_Prototype(szMaskProtos[i]));
		// Mask textures are optional, no error if not found
	}

	return S_OK;
}

CTile* CTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::TILEDATA& tileData)
{
	CTile* pTile = new CTile(pGraphicDev);

	if (FAILED(pTile->Ready_GameObject()))
	{
		Safe_Release(pTile);
		MSG_BOX("CTile Create Failed");
		return nullptr;
	}

	pTile->Set_TileData(tileData.x, tileData.z, tileData.textureId, tileData.maskFlags);

	return pTile;
}

void CTile::Free()
{
	// Mask textures are not in m_mapComponent, so release them separately
	for (_int i = 0; i < 4; ++i)
	{
		Safe_Release(m_pMaskTextures[i]);
	}

	// m_pBufferCom, m_pTransformCom, m_pTextureCom are in m_mapComponent
	// They will be released by CGameObject::Free()
	CGameObject::Free();
}
