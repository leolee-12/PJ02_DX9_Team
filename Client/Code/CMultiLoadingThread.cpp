#include "pch.h"
#include "CMultiLoadingThread.h"

CMultiLoadingThread::CMultiLoadingThread()
    : m_bFinish(false), m_eLoading(LOADING_END)
{
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
    m_iTotalProtoCount = 0;

    // ===== 텍스처 큐 등록 (멀티스레드 로딩) =====
    m_ProtoLoadingqueue.push(TEXLR(L"Proto_ItemTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Item/Item_%02d.png", 8));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_DungeonLine", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/Line%d.png", 2));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_DungeonIcon", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/Icon.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_DungeonBG", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/BG.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_DungeonMG", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/MG2.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_TerrainTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_TerrainWallTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_SkyTexture", TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_EffectTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Explosion/Explosion%d.png", 90));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_PlayerHP", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/Health_%d.png", 3));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_MonsterN1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal1/MonsterN1_%02d.png", 7));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_MonsterN2Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/MonsterN2_%02d.png", 3));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_N2Node1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/N2Node_00.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_N2Node2Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/N2Node_01.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_N2Node3Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/N2Node_02.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_MonsterN3Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal3/MonsterN3_%02d.png", 3));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_GaugeCover", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/GaugeCover.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_GaugeStar", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/GaugeStar.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_PassionGauge", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/PassionGauge.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_PassionIcon", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/PassionIcon_%d.png", 2));
    m_iTotalProtoCount++;

    // ===== TextureSet 큐 등록 (멀티스레드 로딩) =====
    vector<TEXSETLR> vecBishopLeshy;
    vecBishopLeshy.push_back(TEXSETLR(L"Bishop_Leshy_Idle", L"../Bin/Resource/Texture/Bishops/Leshy/Leshy-idle/Leshy_idle-%d.png", 500));
    vecBishopLeshy.push_back(TEXSETLR(L"Bishop_Leshy_Talk", L"../Bin/Resource/Texture/Bishops/Leshy/Leshy-talk/Leshy-talk-%d.png", 167));
    m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_BishopLeshy", TEX_NORMAL), vecBishopLeshy));
    m_iTotalProtoCount++;
}

void CMultiLoadingThread::Loading_for_KnuckleBone()
{
    m_iTotalProtoCount = 0;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_TTutorial", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/TTutorial_%d.png", 2));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_KBCenter", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/KnucklebonesIcon.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_Divider", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/TileDivider_%d.png", 2));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_KBMask", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Knucklebones_BW_MASK_%d.png", 2));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_KBTab", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/TabRed.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_KBSix", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Six.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_KBDice", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Dice_%d.png", 6));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_KBBoardSlot", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/Knucklebones_WhiteSquare.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_KBDiceBox", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/DiceBox_A.png", 1));
    m_iTotalProtoCount++;

    m_ProtoLoadingqueue.push(TEXLR(L"Proto_KBBack", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/DiceTubBG.png", 1));
    m_iTotalProtoCount++;

    //m_iTotalProtoCount = m_ProtoLoadingqueue.unsafe_size();
}

unsigned int CMultiLoadingThread::Thread_Main(void* pArg)
{
    CMultiLoadingThread* pOwner = reinterpret_cast<CMultiLoadingThread*>(pArg);

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

        pOwner->m_TexSetReadyqueue.push(std::move(request));
    }



    while (!pOwner->m_ProtoLoadingqueue.empty()) {
        TEXLR request;
        if (!pOwner->m_ProtoLoadingqueue.try_pop(request))
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

        pOwner->m_ProtoReadyqueue.push(std::move(request));
    }

    pOwner->m_bFinish = true;

    return 0;
}

CMultiLoadingThread* CMultiLoadingThread::Create(LOADINGID eID)
{
    CMultiLoadingThread* pLoading = new CMultiLoadingThread();

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
