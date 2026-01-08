#include "pch.h"
#include "CLoadingThread.h"
#include "CProtoMgr.h"
#include "CPlayer.h"
#include "CPersistentMgr.h"
#include "CN1_AI.h"

CLoadingThread::CLoadingThread(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev), m_bFinish(false), m_eLoading(LOADING_END), m_fPercent(0.f)
{
    ZeroMemory(m_szLoading, sizeof(m_szLoading));
    m_pGraphicDev->AddRef();
}

CLoadingThread::~CLoadingThread()
{
}

HRESULT CLoadingThread::Ready_Loading(LOADINGID eID)
{
    InitializeCriticalSection(&m_Crt);

    m_eLoading = eID;

    int iNumber = 10;

    m_hThread = (HANDLE)_beginthreadex(NULL,    // 보안속성(핸들의 상속 여부, null인 경우 상속에서 제외)
                                        0,      // 디폴트 스택 사이즈(1 바이트)
                                        Thread_Main,      // 쓰레드 함수
                                        this,   // 쓰레드 함수를 이용하여 가공할 데이터 주소    
                                        0,      // 쓰레드 생성 및 실행을 조정하기 옵션(flag)
                                        NULL);  // 쓰레드 id


    return S_OK;
}

_uint CLoadingThread::Loading_ForStage()
{
    //Sleep(1000);

    if (FAILED(CPersistentMgr::GetInstance()->Ready_GlobalObjects(m_pGraphicDev)))
        return E_FAIL;

    m_fPercent += 50.f;

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DungeonLine", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/Line%d.png", 2));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DungeonIcon", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/Icon.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DungeonBG", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/BG.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DungeonMG", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/MG2.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainWallTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_EffectTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV, L"../Bin/Resource/Texture/Terrain/Height.bmp"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainWallTex", Engine::CTerrainWallTex::Create(m_pGraphicDev, 51, 51, VTXITV));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerHP", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/Health_%d.png", 3));

    //CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerHalfHP", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/Health_Half_%d.png", 2));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MonsterN1Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal1/MonsterN1_%02d.png", 7));


    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_N1_AI", CN1_AI::Create(m_pGraphicDev, 5.f, 1.f));

    // ===== 타일 관련 =====
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTexXZ",
        Engine::CRcTexXZ::Create(m_pGraphicDev));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TileTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Tile"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TileMaskTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/TileMasking"));

    // ===== Grass 관련 =====
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GrassBuffer",
        Engine::CGrassBuffer::Create(m_pGraphicDev));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GrassTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Object/Grass"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Collider",
        Engine::CCollider::Create(m_pGraphicDev));

    // ===== 맵 오브젝트 텍스처 =====
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RockTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Object/Rock"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TreeTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Object/Tree"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FireTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Object/Fire"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TentTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Object/Tent"));

    //Sleep(2000);

    m_fPercent += 50.f;

    m_bFinish = true;

    return _uint();
}

_uint CLoadingThread::Loading_ForTest()
{
    //Sleep(1000);

    if (FAILED(CPersistentMgr::GetInstance()->Ready_GlobalObjects(m_pGraphicDev)))
        return E_FAIL;

    m_fPercent += 50.f;

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DungeonLine", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/Line%d.png", 2));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DungeonIcon", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/Icon.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DungeonBG", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/BG.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DungeonMG", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/MG2.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainWallTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2));
    
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4));
    
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_EffectTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90));
    
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev));
    
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV, L"../Bin/Resource/Texture/Terrain/Height.bmp"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainWallTex", Engine::CTerrainWallTex::Create(m_pGraphicDev, 51, 51, VTXITV));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerHP", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/Health_%d.png", 3));

    //CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerHalfHP", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/Health_Half_%d.png", 2));
    
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MonsterN1Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal1/MonsterN1_%02d.png", 7));


    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_N1_AI", CN1_AI::Create(m_pGraphicDev, 5.f, 1.f));

    // ===== 타일 관련 =====
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTexXZ",
        Engine::CRcTexXZ::Create(m_pGraphicDev));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TileTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Tile"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TileMaskTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/TileMasking"));

    // ===== Grass 관련 =====
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GrassBuffer",
        Engine::CGrassBuffer::Create(m_pGraphicDev));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GrassTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Object/Grass"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Collider",
        Engine::CCollider::Create(m_pGraphicDev));

    // ===== 맵 오브젝트 텍스처 =====
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RockTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Object/Rock"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TreeTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Object/Tree"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FireTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Object/Fire"));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TentTexture",
        Engine::CTexture::CreateFromFolder(m_pGraphicDev, TEX_NORMAL,
            L"../../Maps/Texture/Object/Tent"));

    //Sleep(2000);

    m_fPercent += 50.f;
    
    m_bFinish = true;

    return _uint();
}

_uint CLoadingThread::Loading_ForKnuckleBone()
{

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TTutorial", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/TTutorial_%d.png", 2));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KBCenter", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/KnucklebonesIcon.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Divider", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/TileDivider_%d.png", 2));
    
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KBMask", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Knucklebones_BW_MASK.png", 2));

    m_fPercent = 100.f;

    m_bFinish = true;

    return _uint();
}

unsigned int CLoadingThread::Thread_Main(void* pArg)
{
    CLoadingThread* pLoading = reinterpret_cast<CLoadingThread*>(pArg);

    _uint iFlag(0);

    EnterCriticalSection(pLoading->Get_Crt());

    switch (pLoading->Get_Loading())
    {
    case LOADING_STAGE:
        iFlag = pLoading->Loading_ForStage();
        break;

    case LOADING_TEST:
        iFlag = pLoading->Loading_ForTest();
        break;

    case LOADING_KNUCKLEBONE:
        iFlag = pLoading->Loading_ForKnuckleBone();
        break;

    case LOADING_BOSS:
        break;        
    }

    LeaveCriticalSection(pLoading->Get_Crt());

    // _endthreadex(0);

    return iFlag;
}

CLoadingThread* CLoadingThread::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
    CLoadingThread* pLoading = new CLoadingThread(pGraphicDev);

    if (FAILED(pLoading->Ready_Loading(eID)))
    {
        Safe_Release(pLoading);
        MSG_BOX("Loading Create Failed");
        return nullptr;
    }

    return pLoading;
}

void CLoadingThread::Free()
{
    WaitForSingleObject(m_hThread, INFINITE);

    CloseHandle(m_hThread);

    DeleteCriticalSection(&m_Crt);

    Safe_Release(m_pGraphicDev);
}
