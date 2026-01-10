#include "pch.h"
#include "CLoadingThread.h"
#include "CLoading.h"
#include "CProtoMgr.h"
#include "CStage.h"
#include "CManagement.h"
#include "CFontMgr.h"
#include "CTest.h"
#include "CLoadingBack.h"
#include "CLoadingCenter.h"
#include "CLoadingFG.h"
#include "CLoadingCircle.h"
#include "CLoadingLogo.h"
#include "CKnuckleBone.h"
#include "CDungeon.h"
#include "CMultiLoadingThread.h"
#include "Engine_Define.h"
#include "CPersistentMgr.h"
#include "CN1_AI.h"
#include "CN2_AI.h"
#include "CN3_AI.h"
#include "LoadObjectList.h"



CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID)
	: CScene(pGraphicDev), m_pLoading(nullptr), m_eChangeID(ChangeID), m_fLoadingDelay(0.f)
	, m_iCompletedCount(0), m_iLoadingFrame(0)
{
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Scene()
{
	//테스트용
	m_pMessageChannel = CStageMessage::Create();

	if (FAILED(Ready_UI_Layer(L"UI_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	//m_pLoading = CLoadingThread::Create(m_pGraphicDev, m_eChangeID);

	m_pLoading = CMultiLoadingThread::Create(m_eChangeID);

	if (nullptr == m_pLoading)
		return E_FAIL;

	m_iTotalCount = m_pLoading->Get_TotalCount();

	return S_OK;
}

_int CLoading::Update_Scene(const _float& fTimeDelta)
{
	Excute_Ready_Texture();
	m_iCompletedCount;
	m_iTotalCount;

	m_pLoadingFG->Update_Pos(_float(m_iCompletedCount) / _float(m_iTotalCount));

	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);

	//if (true == m_pLoading->Get_Finish())
	if (m_iTotalCount == m_iCompletedCount)
	{
		if (m_fLoadingDelay >= 1.f) {
			Engine::CScene* pScene = nullptr;
			switch (m_pLoading->Get_Loading())
			{
			case LOADING_STAGE:
				pScene = CStage::Create(m_pGraphicDev);

				if (nullptr == pScene)
					return -1;
				break;

			case LOADING_TEST:
				pScene = CTest::Create(m_pGraphicDev);

				if (nullptr == pScene)
					return -1;
				break;

			case LOADING_KNUCKLEBONE:
				pScene = CKnuckleBone::Create(m_pGraphicDev);

				if (nullptr == pScene)
					return -1;
				break;

			case LOADING_DUNGEON:
				pScene = CDungeon::Create(m_pGraphicDev);

				if (nullptr == pScene)
					return -1;
				break;

			case LOADING_BOSS:
				break;
			}

			if (FAILED(CManagement::GetInstance()->Set_Scene(pScene)))
			{
				MSG_BOX("Stage Scene Failed");
				return -1;
			}
		}
		_float fClampedDelta = min(fTimeDelta, 0.1f);  // 최대 100ms로 제한
		m_fLoadingDelay += fClampedDelta;
	}

	return iExit;
}

void CLoading::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);

	//if (true == m_pLoading->Get_Finish()) {
	//	IMessageChannel::EVENT event; 
	//	event.strType = L"Loading_Success";
	//
	//	m_pMessageChannel->Publish(event);
	//}
}

void CLoading::Render_Scene()
{
	// debug 용


	_vec2		vPos{ 150.f, WINCY - 100.f };

	if (true == m_pLoading->Get_Finish()) {
		if (m_iTotalCount == m_iCompletedCount) {
			CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"로딩 완료!", &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
		}
		else {
			CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"오브젝트 생성중", &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
		}
	}
	else {
		CFontMgr::GetInstance()->Render_Font(L"Font_Lapture40", L"텍스쳐 생성중", &vPos, D3DXCOLOR(0.75f, 0.75f, 0.75f, 1.f), DT_NOCLIP);
	}
}


HRESULT CLoading::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CLoading::Ready_UI_Layer(const _tchar* pLayerTag)
{

	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CLoadingBack::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingBack", pGameObject)))
		return E_FAIL;

	pGameObject = CLoadingCenter::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingCenter", pGameObject)))
		return E_FAIL;

	pGameObject = m_pLoadingFG = CLoadingFG::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingFG", pGameObject)))
		return E_FAIL;

	pGameObject =  CLoadingLogo::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"CLoadingLogo", pGameObject)))
		return E_FAIL;

	pGameObject = CLoadingCircle::Create(m_pGraphicDev, m_pMessageChannel);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"LoadingCircle", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CLoading::Ready_Prototype()
{
	CPersistentMgr::GetInstance()->Ready_GlobalObjects(m_pGraphicDev);
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV, L"../Bin/Resource/Texture/Terrain/Height.bmp"));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainWallTex", Engine::CTerrainWallTex::Create(m_pGraphicDev, 51, 51, VTXITV));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTexXZ", Engine::CRcTexXZ::Create(m_pGraphicDev));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TileTexture", Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Maps/Texture/Tile"));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TileMaskTexture", Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Maps/Texture/TileMasking"));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GrassBuffer", Engine::CGrassBuffer::Create(m_pGraphicDev));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Collider", Engine::CCollider::Create(m_pGraphicDev));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_N1_AI", CN1_AI::Create(m_pGraphicDev, 6.f, 1.f));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_N2_AI", CN2_AI::Create(m_pGraphicDev, 6.f, 4.f));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_N3_AI", CN3_AI::Create(m_pGraphicDev, 6.f, 3.f));

	Load_Object_Texture(Tutorial_Texture);
	return S_OK;
}

void CLoading::Load_Object_Texture(const std::set<std::wstring>& textureSet)
{
	for (const auto& name : textureSet)
	{
		auto it = ObjectList.find(name);
		if (it == ObjectList.end() || it->second <= 0)
			continue;

		std::wstring protoName = L"Proto_" + name + L"Texture";
		g_MapProtoname.push_back(protoName);

		std::wstring path = L"../Bin/Resource/Maps/Texture/Object/" + name + L"/" + name + L"%d.png";

		CProtoMgr::GetInstance()->Ready_Prototype(
			g_MapProtoname.back().c_str(),
			Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, path.c_str(), it->second));
	}
}

HRESULT CLoading::Excute_Ready_Texture()
{
	if (m_iLoadingFrame < 5) 
	{
		++m_iLoadingFrame; 
		return S_OK;
	}

	Engine::TEXLR req;
	pair<Engine::TEXSETINFO, vector<Engine::TEXSETLR>> TexSetReq;
	if (m_pLoading->Get_ReadyQueue().try_pop(req)) {

		CProtoMgr::GetInstance()->Ready_Prototype(req.strProtoName.c_str(), CTexture::CreateFromMemory(m_pGraphicDev, req.eTexType, req.vecTexBuffer, req.iTexIndex));

		m_iCompletedCount++;
	}
	else if (m_pLoading->Get_TexSetReadyqueue().try_pop(TexSetReq)) {

		Engine::TEXSETINFO TexSetinfo = std::move(TexSetReq.first);

		vector<Engine::TEXSETLR> vecTexSetLR = std::move(TexSetReq.second);


		CProtoMgr::GetInstance()->Ready_Prototype(TexSetinfo.strProtoName.c_str(), CTextureSet::CreateFromMemory(m_pGraphicDev, TexSetinfo.eTexType, vecTexSetLR));

		m_iCompletedCount++;
	}

	m_iLoadingFrame = 0;

	return S_OK;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID)
{
	CLoading* pLoading = new CLoading(pGraphicDev, ChangeID);

	if (FAILED(pLoading->Ready_Scene()))
	{
		Safe_Release(pLoading);
		MSG_BOX("pLoading Create Failed");
		return nullptr;
	}

	return pLoading;
}

void CLoading::Free()
{
	Safe_Release(m_pLoading);

	CScene::Free();
}
