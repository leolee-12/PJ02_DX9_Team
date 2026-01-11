#include "pch.h"
#include "CMultiLoadingThread.h"
#include "CProtoMgr.h"
#include "CPersistentMgr.h"
#include "CN1_AI.h"
#include "CN2_AI.h"
#include "CN3_AI.h"
#include "CB1_AI.h"
#include "LoadObjectList.h"

CMultiLoadingThread::CMultiLoadingThread(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_eLoading(LOADING_END)
    , m_pGraphicDev(pGraphicDev)
{
    m_pGraphicDev->AddRef();
    for (_int i = 0; i < WORKER_COUNT; ++i)
    {
        m_hThread[i] = nullptr;
    }
}

CMultiLoadingThread::~CMultiLoadingThread()
{
}

HRESULT		CMultiLoadingThread::Ready_Loading(LOADINGID eID)
{
    m_eLoading = eID;

    int iNumber = 10;

    Ready_Loadingqueue();

    for (_int i = 0; i < WORKER_COUNT; ++i)
    {
        m_hThread[i] = (HANDLE)_beginthreadex(NULL,
            0,
            Thread_Main,
            this,
            0,
            NULL);
        if (m_hThread[i] == 0)
        {
            MSG_BOX("멀티 스레드 생성 실패");
            return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CMultiLoadingThread::Ready_Loadingqueue()
{
    switch (m_eLoading)
    {
    case LOADING_STAGE:

        break;

    case LOADING_TEST:

        break;

    case LOADING_KNUCKLEBONE:
        Loading_for_KnuckleBone();
        break;

    case LOADING_DUNGEON:
        Loading_for_Dungeon();
        break;

    case LOADING_BOSS:
        break;

    default:
        MSG_BOX("스테이지 엔트리 플래그값 오류");
        return E_FAIL;
    }

    return S_OK;
}

void CMultiLoadingThread::Loading_for_Dungeon()
{
    m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_Dungeon);

    m_iTotalProtoCount = 0;

    // ===== 텍스처 큐 등록 (멀티스레드 로딩) =====
    m_TexturLoadingqueue.push(TEXLR(L"Proto_ItemTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Item/Item_%02d.png", 8));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_DungeonLine", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/Line%d.png", 2));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_DungeonIcon", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/Icon.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_DungeonBG", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/BG.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_DungeonMG", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/MG2.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_TerrainTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_TerrainWallTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_SkyTexture", TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_EffectTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_PlayerHP", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/Health_%d.png", 3));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_MonsterN1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal1/MonsterN1_%02d.png", 7));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_MonsterN2Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/MonsterN2_%02d.png", 3));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_MonsterN3Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal3/MonsterN3_%02d.png", 3));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_MonsterB1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Boss1/MonsterB1_%02d.png", 8));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_N2Node1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/N2Node_00.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_N2Node2Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/N2Node_01.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_N2Node3Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/N2Node_02.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_B1Node1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Boss1/B1Node_00.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_B1Node2Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Boss1/B1Node_01.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_B1Node3Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Boss1/B1Node_02.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_B1Node4Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Boss1/B1Node_03.png", 1));
    m_iTotalProtoCount++;



    m_TexturLoadingqueue.push(TEXLR(L"Proto_GaugeCover", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/GaugeCover.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_GaugeStar", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/GaugeStar.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_PassionGauge", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/PassionGauge.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_PassionIcon", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/PassionIcon_%d.png", 2));
    m_iTotalProtoCount++;

    // ===== TextureSet 큐 등록 (멀티스레드 로딩) =====
    vector<TEXSETLR> vecBishopLeshy;
    vecBishopLeshy.push_back(TEXSETLR(L"Bishop_Leshy_Idle", L"../Bin/Resource/Texture/Bishops/Leshy/Leshy-idle/Leshy_idle-%d.png", 500));
    vecBishopLeshy.push_back(TEXSETLR(L"Bishop_Leshy_Talk", L"../Bin/Resource/Texture/Bishops/Leshy/Leshy-talk/Leshy-talk-%d.png", 167));
    m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_BishopLeshy", TEX_NORMAL), vecBishopLeshy));
    m_iTotalProtoCount++;

    vector<TEXSETLR> vecBishopHeket;
    vecBishopHeket.push_back(TEXSETLR(L"Bishop_Heket_Idle", L"../Bin/Resource/Texture/Bishops/Heket/Heket-idle/Heket-idle-%d.png", 267));
    vecBishopHeket.push_back(TEXSETLR(L"Bishop_Heket_Talk", L"../Bin/Resource/Texture/Bishops/Heket/Heket-talk/Heket-talk-%d.png", 267));
    m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_BishopHeket", TEX_NORMAL), vecBishopHeket));
    m_iTotalProtoCount++;

    vector<TEXSETLR> vecBishopKallamar;
    vecBishopKallamar.push_back(TEXSETLR(L"Bishop_Kallamar_Idle", L"../Bin/Resource/Texture/Bishops/Kallamar/Kallamar-idle/Kallamar-idle-%d.png", 134));
    vecBishopKallamar.push_back(TEXSETLR(L"Bishop_Kallamar_Talk", L"../Bin/Resource/Texture/Bishops/Kallamar/Kallamar-talk/Kallamar-talk-%d.png", 267));
    m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_BishopKallamar", TEX_NORMAL), vecBishopKallamar));
    m_iTotalProtoCount++;

    vector<TEXSETLR> vecBishopShamura;
    vecBishopShamura.push_back(TEXSETLR(L"Bishop_Shamura_Idle", L"../Bin/Resource/Texture/Bishops/Shamura/Shamura-idle/Shamura-idle-%d.png", 134));
    vecBishopShamura.push_back(TEXSETLR(L"Bishop_Shamura_Talk", L"../Bin/Resource/Texture/Bishops/Shamura/Shamura-talk/Shamura-talk-%d.png", 267));
    m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_BishopShamura", TEX_NORMAL), vecBishopShamura));
    m_iTotalProtoCount++;

    //m_iTotalProtoCount = m_TexturLoadingqueue.unsafe_size();
}

void CMultiLoadingThread::Loading_for_KnuckleBone()
{
    m_iTotalProtoCount = 0;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_TTutorial", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/TTutorial_%d.png", 2));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_KBCenter", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/KnucklebonesIcon.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_Divider", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/TileDivider_%d.png", 2));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_KBMask", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Knucklebones_BW_MASK_%d.png", 2));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_KBTab", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/TabRed.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_KBSix", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Six.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_KBDice", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Dice_%d.png", 6));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_KBBoardSlot", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Knucklebones_WhiteSquare.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_KBDiceBox", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/DiceBox_A.png", 1));
    m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_KBBack", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/DiceTubBG.png", 1));
    m_iTotalProtoCount++;

    //m_iTotalProtoCount = m_TexturLoadingqueue.unsafe_size();
}

void CMultiLoadingThread::NonTex_for_Dungeon()
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
    CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_B1_AI", CB1_AI::Create(m_pGraphicDev, 40.f, 5.f));

    Load_Object_Texture(Tutorial_Texture);
}

void CMultiLoadingThread::Load_Object_Texture(const std::set<std::wstring>& textureSet)
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

unsigned int CMultiLoadingThread::Thread_Main(void* pArg)
{
    CMultiLoadingThread* pOwner = reinterpret_cast<CMultiLoadingThread*>(pArg);

    while (!pOwner->m_NonTexReadyQueue.empty()) {
        TaskFunc Func = nullptr;
        if (!pOwner->m_NonTexReadyQueue.try_pop(Func))
            continue;

        (pOwner->*Func)();
    }

    while (!pOwner->m_TexSetLoadingqueue.empty()) {
        pair<TEXSETINFO, vector<TEXSETLR>> request;
        if (!pOwner->m_TexSetLoadingqueue.try_pop(request))
            continue;

        for (TEXSETLR& TexSetRequest : request.second)
        {
            for (int i = 0; i < TexSetRequest.iTexIndex; ++i) {
                wchar_t szPath[256];
                wsprintf(szPath, TexSetRequest.strFilepath.c_str(), i);

                FILE* fp;
                errno_t err = _wfopen_s(&fp, szPath, L"rb");
                if (err != 0 || !fp)
                {
                    return 1;
                }
                fseek(fp, 0, SEEK_END);
                size_t size = ftell(fp);
                fseek(fp, 0, SEEK_SET);

                vector<BYTE> buffer(size);
                fread(buffer.data(), 1, size, fp);
                fclose(fp);

                TexSetRequest.vecTexBuffer.push_back(std::move(buffer));
            }
        }

        pOwner->m_iLoadCount++;

        pOwner->m_TexSetReadyqueue.push(std::move(request));
    }

    while (!pOwner->m_TexturLoadingqueue.empty()) {
        TEXLR request;
        if (!pOwner->m_TexturLoadingqueue.try_pop(request))
            continue;

        for (int i = 0; i < request.iTexIndex; ++i) {
            wchar_t szPath[256];
            wsprintf(szPath, request.strFilepath.c_str(), i);

            FILE* fp;
            errno_t err = _wfopen_s(&fp, szPath, L"rb");
            if (err != 0 || !fp)
            {
                return 1;
            }
            fseek(fp, 0, SEEK_END);
            size_t size = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            vector<BYTE> buffer(size);
            fread(buffer.data(), 1, size, fp);
            fclose(fp);

            request.vecTexBuffer.push_back(std::move(buffer));
        }

        pOwner->m_iLoadCount++;

        pOwner->m_TextureReadyqueue.push(std::move(request));
        
    }

    while (!pOwner->m_TexSetReadyqueue.empty()) {
        pair<Engine::TEXSETINFO, vector<Engine::TEXSETLR>> TexSetReq;
        if (!pOwner->m_TexSetReadyqueue.try_pop(TexSetReq))
            continue;

        Engine::TEXSETINFO TexSetinfo = std::move(TexSetReq.first);

		vector<Engine::TEXSETLR> vecTexSetLR = std::move(TexSetReq.second);

        CTextureSet* Test = CTextureSet::CreateFromMemory(pOwner->m_pGraphicDev, TexSetinfo.eTexType, vecTexSetLR);

		CProtoMgr::GetInstance()->Ready_Prototype(TexSetinfo.strProtoName.c_str(), Test);

		pOwner->m_iCompletedCount++;
    }

    while (!pOwner->m_TextureReadyqueue.empty()) {
        Engine::TEXLR req;
        if (!pOwner->m_TextureReadyqueue.try_pop(req))
            continue;

        CProtoMgr::GetInstance()->Ready_Prototype(req.strProtoName.c_str(), CTexture::CreateFromMemory(pOwner->m_pGraphicDev, req.eTexType, req.vecTexBuffer, req.iTexIndex));

        pOwner->m_iCompletedCount++;
    }

    //pOwner->m_bFinish = true;

    return 0;
}

CMultiLoadingThread* CMultiLoadingThread::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
    CMultiLoadingThread* pLoading = new CMultiLoadingThread(pGraphicDev);

    if (FAILED(pLoading->Ready_Loading(eID)))
    {
        Safe_Release(pLoading);
        MSG_BOX("Loading Create Failed");
        return nullptr;
    }

    return pLoading;
}

void CMultiLoadingThread::Free()
{
    WaitForMultipleObjects(WORKER_COUNT, m_hThread, TRUE, INFINITE);

    for (_uint i = 0; i < WORKER_COUNT; ++i) {
        CloseHandle(m_hThread[i]);
    }
}
