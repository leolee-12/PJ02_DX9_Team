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

    m_hThread = (HANDLE)_beginthreadex(NULL,    
                                        0,      
                                        Thread_Main,      
                                        this,       
                                        0,      
                                        NULL);  


    return S_OK;
}

_uint CLoadingThread::Loading_ForStage()
{
    lstrcpy(m_szLoading, L"Buffer Loading..............................");
 
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV, L"../Bin/Resource/Texture/Terrain/Height.bmp"))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainWallTex", Engine::CTerrainWallTex::Create(m_pGraphicDev, 51, 51, VTXITV))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
        return E_FAIL;

    m_fPercent += 20.f;
        
    lstrcpy(m_szLoading, L"Texture Loading..............................");

    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Player/idle/idle_%04d.png", 180))))
    //    return E_FAIL;

    /*vector<CTextureSet::TEXINFO> tempVec(16);
    tempVec[0]  = CTextureSet::TEXINFO(L"idle", L"../Bin/Resource/Texture/Player/idle/idle_%04d.png", 72);
    tempVec[1]  = CTextureSet::TEXINFO(L"idle-up", L"../Bin/Resource/Texture/Player/idle-up/idle-up_%04d.png", 72);
    tempVec[2]  = CTextureSet::TEXINFO(L"run-up", L"../Bin/Resource/Texture/Player/run-up/run-up_%04d.png", 9);
    tempVec[3]  = CTextureSet::TEXINFO(L"run-down", L"../Bin/Resource/Texture/Player/run-down/run-down_%04d.png", 9);
    tempVec[4]  = CTextureSet::TEXINFO(L"run-diagonal", L"../Bin/Resource/Texture/Player/run/run_%04d.png", 9);
    tempVec[5]  = CTextureSet::TEXINFO(L"run-horizontal", L"../Bin/Resource/Texture/Player/run-horizontal/run-horizontal_%04d.png", 9);
    tempVec[6]  = CTextureSet::TEXINFO(L"run-up-diagonal", L"../Bin/Resource/Texture/Player/run-up-diagonal/run-up-diagonal_%04d.png", 9);
    tempVec[7]  = CTextureSet::TEXINFO(L"roll-up", L"../Bin/Resource/Texture/Player/roll-up/roll-up_%04d.png", 8);
    tempVec[8]  = CTextureSet::TEXINFO(L"roll-down", L"../Bin/Resource/Texture/Player/roll-down/roll-down_%04d.png", 8);
    tempVec[9]  = CTextureSet::TEXINFO(L"roll-horizontal", L"../Bin/Resource/Texture/Player/roll/roll_%04d.png", 9);
    tempVec[10] = CTextureSet::TEXINFO(L"attack-combo1", L"../Bin/Resource/Texture/Player/attack-combo1/attack-combo1_%04d.png", 15);
    tempVec[11] = CTextureSet::TEXINFO(L"attack-combo2", L"../Bin/Resource/Texture/Player/attack-combo2/attack-combo2_%04d.png", 19);
    tempVec[12] = CTextureSet::TEXINFO(L"attack-combo3", L"../Bin/Resource/Texture/Player/attack-combo3/attack-combo3_%04d.png", 19);
    tempVec[13] = CTextureSet::TEXINFO(L"charge-start", L"../Bin/Resource/Texture/Player/cast-spell2/cast-spell2_%04d.png", 16);
    tempVec[14] = CTextureSet::TEXINFO(L"charge-loop", L"../Bin/Resource/Texture/Player/cast-spell2-loop/cast-spell2-loop_%04d.png", 13);
    tempVec[15] = CTextureSet::TEXINFO(L"charge-end", L"../Bin/Resource/Texture/Player/cast-spell/cast-spell_%04d.png", 16);*/

    m_fPercent += 20.f;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MonsterTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster0.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MonsterN1Texture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal1/MonsterN1_%02d.png", 7))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainWallTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_EffectTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90))))
        return E_FAIL;

    m_fPercent += 20.f;
   
    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/159.dds", 1))))
    //    return E_FAIL;

    lstrcpy(m_szLoading, L"Etc Loading..............................");

    if(FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_N1_AI", CN1_AI::Create(m_pGraphicDev, 5.f, 1.f))))
		return E_FAIL;

    if (FAILED(CPersistentMgr::GetInstance()->Ready_GlobalObjects(m_pGraphicDev)))
        return E_FAIL;

    m_fPercent += 20.f;

    m_fPercent += 20.f;


    m_bFinish = true;

    lstrcpy(m_szLoading, L"�׽�Ʈ");

    return 0;
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
    
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KBMask", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Knucklebones_BW_MASK_%d.png", 2));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KBTab", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/TabRed.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KBSix", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Six.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KBDice", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Dice_%d.png", 6));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KBBoardSlot", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Knucklebones_WhiteSquare.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KBDiceBox", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/DiceBox_A.png", 1));

    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KBBack", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/DiceTubBG.png", 1));


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
