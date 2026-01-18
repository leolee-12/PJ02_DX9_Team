#include "CRenderer.h"
#include "CCollider.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
	Free();
}

void CRenderer::Add_RenderGroup(RENDERID eType, CGameObject* pGameObject)
{
	if (RENDER_END <= eType || nullptr == pGameObject || eType < 0)
		return;

	m_RenderGroup[eType].push_back(pGameObject);
	pGameObject->AddRef();
}

void CRenderer::Add_ColliderDebugGroup(CCollider* pCollider)
{
	if (nullptr == pCollider)
		return;
	m_vecColDebugGroup.push_back(pCollider);
	pCollider->AddRef();
}

void CRenderer::Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_Tile(pGraphicDev);
	Render_Floor(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_UI(pGraphicDev);
	Render_Font(pGraphicDev);

	Render_ColliderDebug(pGraphicDev);

	Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}

	for (auto& pCollider : m_vecColDebugGroup)
		Safe_Release(pCollider);

	m_vecColDebugGroup.clear();

	for (auto iter = m_vecTestColliders.begin();
		iter != m_vecTestColliders.end();)
	{
		if (iter->iFrame == 0)
		{
			iter = m_vecTestColliders.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CRenderer::Add_TestCollider(const AABB& tAABB, _int iFrame)
{
	TESTCOL tTestCol = { tAABB, iFrame };
	m_vecTestColliders.push_back(tTestCol);
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& pObj : m_RenderGroup[RENDER_PRIORITY])
		pObj->Render_GameObject();
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& pObj : m_RenderGroup[RENDER_NONALPHA])
		pObj->Render_GameObject();
}

void CRenderer::Render_Tile(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x08);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);


	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	for (auto& pObj : m_RenderGroup[RENDER_TILE])
		pObj->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	pGraphicDev->SetMaterial(&tMtrl);


	m_RenderGroup[RENDER_ALPHA].sort([](CGameObject* pDst, CGameObject* pSrc)->bool
		{
			if (pDst->Get_DepthZ() == pSrc->Get_DepthZ())
			{
				return pDst->Get_DepthY() < pSrc->Get_DepthY();
			}
			else
			{
				return pDst->Get_DepthZ() > pSrc->Get_DepthZ();
			}
		});


	for (auto& pObj : m_RenderGroup[RENDER_ALPHA])
		pObj->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	_matrix matOldView, matOldProj;
	pGraphicDev->GetTransform(D3DTS_VIEW, &matOldView);
	pGraphicDev->GetTransform(D3DTS_PROJECTION, &matOldProj);

	_matrix matView, matProj;
	D3DXMatrixIdentity(&matView);
	D3DXMatrixOrthoLH(&matProj, (_float)WINCX, (_float)WINCY, 0.f, 1.f);
		
	pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	m_RenderGroup[RENDER_UI].sort([](CGameObject* pDst, CGameObject* pSrc)->bool
		{
			return pDst->Get_Depth() > pSrc->Get_Depth();
		});

	for (auto& pObj : m_RenderGroup[RENDER_UI])
		pObj->Render_GameObject();

	pGraphicDev->SetTransform(D3DTS_VIEW, &matOldView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matOldProj);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CRenderer::Render_Font(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& pObj : m_RenderGroup[RENDER_FONT])
		pObj->Render_GameObject();
}

void CRenderer::Render_Floor(LPDIRECT3DDEVICE9& pGraphicDev)
{
	m_RenderGroup[RENDER_FLOOR].sort([](CGameObject* pDst, CGameObject* pSrc)->bool
		{
			return pDst->Get_Depth() > pSrc->Get_Depth();
		});

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x08);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	pGraphicDev->SetMaterial(&tMtrl);

	for (auto& pObj : m_RenderGroup[RENDER_FLOOR])
		pObj->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CRenderer::Render_ColliderDebug(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetTexture(0, NULL);
	//pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE); // 조명 끄기

	_matrix matOldWorld, matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	pGraphicDev->GetTransform(D3DTS_WORLD, &matOldWorld);
	pGraphicDev->SetTransform(D3DTS_WORLD, &matIdentity);

	for (auto& pCollider : m_vecColDebugGroup)
	{
		pCollider->Render_Collider();
	}

	for (auto& TestCol : m_vecTestColliders)
	{
		D3DXVECTOR3 c = { TestCol.tAABB.x, TestCol.tAABB.y, TestCol.tAABB.z };
		D3DXVECTOR3 h = { TestCol.tAABB.hx, TestCol.tAABB.hy, TestCol.tAABB.hz };
		// 8개 꼭짓점 계산 
		D3DXVECTOR3 v[8] = {
			{c.x - h.x, c.y - h.y, c.z - h.z},
			{c.x - h.x, c.y - h.y, c.z + h.z},
			{c.x - h.x, c.y + h.y, c.z - h.z},
			{c.x - h.x, c.y + h.y, c.z + h.z},
			{c.x + h.x, c.y - h.y, c.z - h.z},
			{c.x + h.x, c.y - h.y, c.z + h.z},
			{c.x + h.x, c.y + h.y, c.z - h.z},
			{c.x + h.x, c.y + h.y, c.z + h.z},
		};
		// 12개 엣지를 연결하는 인덱스 
		WORD indices[24] =
		{
			0,1, 0,2, 0,4,
			7,6, 7,5, 7,3,
			1,5, 1,3, 2,3,
			2,6, 4,5, 4,6
		};

		DebugVertex verts[8];
		for (int i = 0; i < 8; ++i)
		{
			verts[i].vPosition = v[i];
			verts[i].dwColor = D3DCOLOR_ARGB(255, 0, 255, 0);
		}

		pGraphicDev->SetFVF(FVF_DEBUG);
		pGraphicDev->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 8, 12, indices, D3DFMT_INDEX16, verts, sizeof(DebugVertex));

		--TestCol.iFrame;
	}


	pGraphicDev->SetTransform(D3DTS_WORLD, &matOldWorld);
}

void CRenderer::Free()
{
	Clear_RenderGroup();
}
