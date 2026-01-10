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
	, m_pMaskTextureCom(nullptr)
	, m_iGridX(0)
	, m_iGridZ(0)
	, m_iTextureId(0)
	, m_iMaskFlags(0)
{
}

CTile::CTile(const CTile& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pMaskTextureCom(nullptr)
	, m_iGridX(rhs.m_iGridX)
	, m_iGridZ(rhs.m_iGridZ)
	, m_iTextureId(rhs.m_iTextureId)
	, m_iMaskFlags(rhs.m_iMaskFlags)
{
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
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// Material
	D3DMATERIAL9 tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));
	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	m_pGraphicDev->SetMaterial(&tMtrl);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	// Render base tile
	m_pTextureCom->Set_Texture(m_iTextureId);
	m_pBufferCom->Render_Buffer();

	// Render masks with alpha blending
	if (m_iMaskFlags != Engine::MASK_NONE)
	{
		Render_Masks();
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

	m_pTransformCom->Set_Pos(fCenterX, -2.5f, fCenterZ);
	m_pTransformCom->Set_Scale(fTileSize, 1.f, fTileSize);  // X, Z scale (XZ plane buffer)
}

void CTile::Render_Masks()
{
	// Check if mask texture is loaded
	if (nullptr == m_pMaskTextureCom)
		return;

	DWORD oldAlphaop, oldColorop;

	// Enable alpha blending (standard settings)
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Alpha test to remove low-alpha edge artifacts (0x80 = 128, 50% threshold)
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x80);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	// Mask color = black (use teture alpha only)
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &oldColorop);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &oldAlphaop);

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF000000);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	_matrix matWorld, matScale, matTrans;
	_float fTileSize = Engine::CMapLoader::TILE_SIZE;
	_float fBaseX = static_cast<_float>(m_iGridX) * fTileSize;
	_float fBaseZ = static_cast<_float>(m_iGridZ) * fTileSize;

	// Tile Y position
	const _float fTileY = -2.5f;
	const _float fEdgeRatio = 0.25f;  // 25% of tile size

	// Down mask (X: 2 pieces, Z: 25% at bottom)
	if (m_iMaskFlags & Engine::MASK_DOWN)
	{
		_float fScaleX = fTileSize * 0.5f;
		_float fScaleZ = fTileSize * fEdgeRatio;
		_float fPosZ = fBaseZ + fScaleZ * 0.5f;

		D3DXMatrixScaling(&matScale, fScaleX, 1.f, fScaleZ);
		m_pMaskTextureCom->Set_Texture(0);

		for (_int i = 0; i < 2; ++i)
		{
			_float fPosX = fBaseX + fScaleX * 0.5f + i * fScaleX;
			D3DXMatrixTranslation(&matTrans, fPosX, fTileY + 0.02f, fPosZ);
			matWorld = matScale * matTrans;
			m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
			m_pBufferCom->Render_Buffer();
		}
	}

	// Left mask (X: 25% at left, Z: 2 pieces)
	if (m_iMaskFlags & Engine::MASK_LEFT)
	{
		_float fScaleX = fTileSize * fEdgeRatio;
		_float fScaleZ = fTileSize * 0.5f;
		_float fPosX = fBaseX + fScaleX * 0.5f;

		D3DXMatrixScaling(&matScale, fScaleX, 1.f, fScaleZ);
		m_pMaskTextureCom->Set_Texture(1);

		for (_int i = 0; i < 2; ++i)
		{
			_float fPosZ = fBaseZ + fScaleZ * 0.5f + i * fScaleZ;
			D3DXMatrixTranslation(&matTrans, fPosX, fTileY + 0.021f, fPosZ);
			matWorld = matScale * matTrans;
			m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
			m_pBufferCom->Render_Buffer();
		}
	}

	// Right mask (X: 25% at right, Z: 2 pieces)
	if (m_iMaskFlags & Engine::MASK_RIGHT)
	{
		_float fScaleX = fTileSize * fEdgeRatio;
		_float fScaleZ = fTileSize * 0.5f;
		_float fPosX = fBaseX + fTileSize - fScaleX * 0.5f;

		D3DXMatrixScaling(&matScale, fScaleX, 1.f, fScaleZ);
		m_pMaskTextureCom->Set_Texture(2);

		for (_int i = 0; i < 2; ++i)
		{
			_float fPosZ = fBaseZ + fScaleZ * 0.5f + i * fScaleZ;
			D3DXMatrixTranslation(&matTrans, fPosX, fTileY + 0.022f, fPosZ);
			matWorld = matScale * matTrans;
			m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
			m_pBufferCom->Render_Buffer();
		}
	}

	// Up mask (X: 2 pieces, Z: 25% at top)
	if (m_iMaskFlags & Engine::MASK_UP)
	{
		_float fScaleX = fTileSize * 0.5f;
		_float fScaleZ = fTileSize * fEdgeRatio;
		_float fPosZ = fBaseZ + fTileSize - fScaleZ * 0.5f;

		D3DXMatrixScaling(&matScale, fScaleX, 1.f, fScaleZ);
		m_pMaskTextureCom->Set_Texture(3);

		for (_int i = 0; i < 2; ++i)
		{
			_float fPosX = fBaseX + fScaleX * 0.5f + i * fScaleX;
			D3DXMatrixTranslation(&matTrans, fPosX, fTileY + 0.023f, fPosZ);
			matWorld = matScale * matTrans;
			m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
			m_pBufferCom->Render_Buffer();
		}
	}

	// Restore TextureStage settings
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, oldColorop);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, oldAlphaop);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
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

	// Mask Texture (Down=0, Left=1, Right=2, Up=3 in alphabetical order)
	m_pMaskTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TileMaskTexture"));
	// Mask texture is optional, no error if not found

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
	// Mask texture is not in m_mapComponent, so release it separately
	Safe_Release(m_pMaskTextureCom);

	// m_pBufferCom, m_pTransformCom, m_pTextureCom are in m_mapComponent
	// They will be released by CGameObject::Free()
	CGameObject::Free();
}
