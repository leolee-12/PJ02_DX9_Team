#include "pch.h"
#include "CLoadingThread.h"
#include "CProtoMgr.h"
#include "CPlayer.h"
#include "CPersistentMgr.h"

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
    lstrcpy(m_szLoading, L"Buffer Loading..............................");

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TriCol", Engine::CTriCol::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcCol", Engine::CRcCol::Create(m_pGraphicDev))))
        return E_FAIL;
 
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV, L"../Bin/Resource/Texture/Terrain/Height.bmp"))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainWallTex", Engine::CTerrainWallTex::Create(m_pGraphicDev, 51, 51, VTXITV))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Collider", Engine::CCollider::Create(m_pGraphicDev))))
        return E_FAIL;
        
    lstrcpy(m_szLoading, L"Texture Loading..............................");

    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Player/idle/idle_%04d.png", 180))))
    //    return E_FAIL;

    vector<CTextureSet::TEXINFO> tempVec(13);
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
    
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerTexture", Engine::CTextureSet::Create(m_pGraphicDev, TEX_NORMAL, tempVec))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MonsterTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Monster0.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainWallTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_EffectTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LoadingCenterTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Loading/LoadingCenter.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LoadingFGTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Loading/LoadingFG.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LoadingLogoTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Loading/LoadingLogo.png", 1))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LoadingCircleTex", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Loading/LoadingCircle.png", 1))))
        return E_FAIL;
   
    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/159.dds", 1))))
    //    return E_FAIL;

    lstrcpy(m_szLoading, L"Etc Loading..............................");

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Calculator", Engine::CCalculator::Create(m_pGraphicDev))))
        return E_FAIL;

    
    if (FAILED(CPersistentMgr::GetInstance()->Ready_GlobalObjects(m_pGraphicDev)))
        return E_FAIL;


    m_bFinish = true;

    lstrcpy(m_szLoading, L"테스트");

    return 0;
}

_uint CLoadingThread::Loading_ForTest()
{
    Sleep(1000);

    m_fPercent += 50.f;

    Sleep(2000);

    lstrcpy(m_szLoading, L"Loading Complete!!");

    m_fPercent += 50.f;
    
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
