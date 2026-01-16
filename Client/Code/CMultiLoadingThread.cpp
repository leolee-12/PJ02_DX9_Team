#include "pch.h"
#include "CMultiLoadingThread.h"
#include "CProtoMgr.h"
#include "CPersistentMgr.h"
#include "LoadObjectList.h"
#include "CN1_AI.h"
#include "CN2_AI.h"
#include "CN3_AI.h"
#include "CB1_AI.h"
#include "CB2_AI.h"

CMultiLoadingThread::CMultiLoadingThread(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_eLoading(LOADING_END)
	, m_pGraphicDev(pGraphicDev)
	, m_iTotalProtoCount(0)
{
	m_pGraphicDev->AddRef();
	m_vecThread.resize(WORKER_COUNT);
	for (_uint i = 0; i < WORKER_COUNT; ++i)
	{
		m_vecThread[i] = nullptr;
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

	for (_uint i = 0; i < WORKER_COUNT; ++i)
	{
		m_vecThread[i] = (HANDLE)_beginthreadex(NULL,
			0,
			Thread_Main,
			this,
			0,
			NULL);
		if (m_vecThread[i] == 0)
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
	case LOADING_LOGO:
		Loading_for_Logo();
		break;

	case LOADING_STAGE:
		Loading_for_Stage();
		break;

	case LOADING_BOSS:
		Loading_for_Boss();
		break;

	case LOADING_TEST:
		Loading_for_Test();
		break;

	case LOADING_KNUCKLEBONE:
		Loading_for_KnuckleBone();
		break;

	case LOADING_DUNGEON:
		Loading_for_Dungeon();
		break;

	case LOADING_TUTORIAL:
		Loading_for_Tutorial();
		break;

	case LOADING_THEGATEWAY:
		Loading_for_TheGateway();
		break;

	case LOADING_VILLAGE:
		Loading_for_Village();
		break;

	case LOADING_REALDUNGEON:
		Loading_for_RealDungeon();
		break;

	case LOADING_AMDUSIASROOM:
		Loading_for_AmdusiasRoom();
		break;

	case LOADING_LESHYROOM:
		Loading_for_LeshyRoom();
		break;

	default:
		MSG_BOX("스테이지 엔트리 플래그값 오류");
		return E_FAIL;
	}

	return S_OK;
}

void CMultiLoadingThread::Loading_for_Logo()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_Logo);
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_MainMenuTex", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Mainmenu(2)/dds/animation_%04d.dds", 48));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_MainLogoTex", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Mainmenu/dds/Logo.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_MainTabTex", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Mainmenu/dds/Tab.dds", 1));
	m_iTotalProtoCount++;
}

void CMultiLoadingThread::Loading_for_Stage()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_Stage);
	m_iTotalProtoCount++;
}

void CMultiLoadingThread::Loading_for_Boss()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_Boss);
	m_iTotalProtoCount++;
}

void CMultiLoadingThread::Loading_for_Test()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_Test);
	m_iTotalProtoCount++;
}

void CMultiLoadingThread::Loading_for_KnuckleBone()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_Tutorial);
	m_iTotalProtoCount++;


	m_TexturLoadingqueue.push(TEXLR(L"Proto_TTutorial", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/TTutorial_%d.dds", 2));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_KBCenter", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/KnucklebonesIcon.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_Divider", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/TileDivider_%d.dds", 2));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_KBMask", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/Knucklebones_BW_MASK_%d.dds", 2));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_KBTab", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/TabRed.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_KBSix", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/Six.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_KBDice", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/Dice_%d.dds", 6));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_KBBoardSlot", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/Knucklebones_WhiteSquare.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_KBDiceBox", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/DiceBox_A.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_KBBack", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/DiceTubBG.dds", 1));
	m_iTotalProtoCount++;

	//m_iTotalProtoCount = m_TexturLoadingqueue.unsafe_size();
}

void CMultiLoadingThread::Loading_for_Tutorial()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_Tutorial);
	m_iTotalProtoCount++;


	// ===== 텍스처 큐 등록 (멀티스레드 로딩) =====

	m_TexturLoadingqueue.push(TEXLR(L"Proto_SkyTexture", TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_MonsterN1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal1/dds/MonsterN1_%02d.dds", 7));
	m_iTotalProtoCount++;

    m_TexturLoadingqueue.push(TEXLR(L"Proto_SpeechBubble", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Etc/dds/SpeechBubble.dds", 1));
    m_iTotalProtoCount++;

    // ===== TextureSet 큐 등록 (멀티스레드 로딩) =====
    vector<TEXSETLR> vecBishopLeshy;
    vecBishopLeshy.push_back(TEXSETLR(L"Bishop_Leshy_Idle", L"../Bin/Resource/Texture/Bishops/Leshy/Leshy-idle/dds/Leshy_idle-%d.dds", 250));
    vecBishopLeshy.push_back(TEXSETLR(L"Bishop_Leshy_Talk", L"../Bin/Resource/Texture/Bishops/Leshy/Leshy-talk/dds/Leshy-talk%d.dds", 83));
    m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_BishopLeshy", TEX_NORMAL), vecBishopLeshy));
    m_iTotalProtoCount++;

	vector<TEXSETLR> vecBishopHeket;
	vecBishopHeket.push_back(TEXSETLR(L"Bishop_Heket_Idle", L"../Bin/Resource/Texture/Bishops/Heket/Heket-idle/dds/Heket-idle-%d.dds", 267));
	vecBishopHeket.push_back(TEXSETLR(L"Bishop_Heket_Talk", L"../Bin/Resource/Texture/Bishops/Heket/Heket-talk/dds/Heket-talk-%d.dds", 267));
	m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_BishopHeket", TEX_NORMAL), vecBishopHeket));
	m_iTotalProtoCount++;

	vector<TEXSETLR> vecBishopKallamar;
	vecBishopKallamar.push_back(TEXSETLR(L"Bishop_Kallamar_Idle", L"../Bin/Resource/Texture/Bishops/Kallamar/Kallamar-idle/dds/Kallamar-idle-%d.dds", 134));
	vecBishopKallamar.push_back(TEXSETLR(L"Bishop_Kallamar_Talk", L"../Bin/Resource/Texture/Bishops/Kallamar/Kallamar-talk/dds/Kallamar-talk-%d.dds", 267));
	m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_BishopKallamar", TEX_NORMAL), vecBishopKallamar));
	m_iTotalProtoCount++;

	vector<TEXSETLR> vecBishopShamura;
	vecBishopShamura.push_back(TEXSETLR(L"Bishop_Shamura_Idle", L"../Bin/Resource/Texture/Bishops/Shamura/Shamura-idle/dds/Shamura-idle-%d.dds", 134));
	vecBishopShamura.push_back(TEXSETLR(L"Bishop_Shamura_Talk", L"../Bin/Resource/Texture/Bishops/Shamura/Shamura-talk/dds/Shamura-talk-%d.dds", 267));
	m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_BishopShamura", TEX_NORMAL), vecBishopShamura));
	m_iTotalProtoCount++;
}

void CMultiLoadingThread::Loading_for_Dungeon()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_Dungeon);
	m_iTotalProtoCount++;


	// ===== 텍스처 큐 등록 (멀티스레드 로딩) =====
	m_TexturLoadingqueue.push(TEXLR(L"Proto_ItemTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Item/dds/Item_%02d.dds", 8));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_DungeonLine", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/dds/Line%d.dds", 2));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_DungeonIcon", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/dds/Icon.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_DungeonBG", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/dds/BG.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_DungeonMG", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Dungeon/dds/MG2.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_SkyTexture", TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_PlayerHP", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/dds/Health_%d.dds", 3));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_MonsterN1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal1/dds/MonsterN1_%02d.dds", 7));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_MonsterN2Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/dds/MonsterN2_%02d.dds", 3));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_MonsterN3Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal3/dds/MonsterN3_%02d.dds", 3));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_MonsterB1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Boss1/dds/MonsterB1_%02d.dds", 10));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_N2Node1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/dds/N2Node_00.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_N2Node2Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/dds/N2Node_01.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_N2Node3Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal2/dds/N2Node_02.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_B1Node1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Boss1/dds/B1Node_00.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_B1Node2Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Boss1/dds/B1Node_01.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_B1Node3Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Boss1/dds/B1Node_02.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_B1Node4Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Boss1/dds/B1Node_03.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_ProjectileTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/dds/Projectile.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_GaugeCover", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/dds/GaugeCover.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_GaugeStar", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/dds/GaugeStar.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_PassionGauge", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/dds/PassionGauge.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_PassionIcon", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/dds/PassionIcon_%d.dds", 2));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingBarkCover", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingMiniGame/dds/CookingBar_Border.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingBarRed", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingMiniGame/dds/CookingBar_Red.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingBarGreen", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingMiniGame/dds/CookingBar_Green.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingMarker", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingMiniGame/dds/Cooking_Marker.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingButton", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingMiniGame/dds/CookingButton.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingTarget", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingMiniGame/dds/FoodTexture_%d.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingTargetBack", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingMiniGame/dds/CookingTargetBack.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingSelectBack", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingSelect/dds/CookingSelectBack.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingSelectSlot", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingSelect/dds/SelectSlot.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingSelectStartBtn", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingSelect/SelectFoodTexture.png", 1));
	m_iTotalProtoCount++;
	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingStar", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingSelect/Star_%d.png", 2));
	m_iTotalProtoCount++;
	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingCardInfoRecipe", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingSelect/Recipe.png", 1));
	m_iTotalProtoCount++;
	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingArrowUpDown", TEX_NORMAL, L"../Bin/Resource/YSD/CookingSelect/FaithArrowUpDown_%d.png", 2));
	m_iTotalProtoCount++;
	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingSelectButton", TEX_NORMAL, L"../Bin/Resource/YSD/CookingSelect/Button_%d.png", 2));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingCardInfoFaithTexture", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingSelect/dds/FaithTexture.dds", 1));
	m_iTotalProtoCount++;
	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingCardInfoCardBack", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingSelect/dds/InfoCardBackground.dds", 1));
	m_iTotalProtoCount++;
	m_TexturLoadingqueue.push(TEXLR(L"Proto_Divider", TEX_NORMAL, L"../Bin/Resource/Texture/UI/KnuckleBone/dds/TileDivider_%d.dds", 2));
	m_iTotalProtoCount++;
	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingCardInfoFaithTexture", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingSelect/dds/FaithTexture.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingCardInfoRightPattern", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingSelect/dds/Rightpattern.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_CookingCardInfoLeftPattern", TEX_NORMAL, L"../Bin/Resource/Texture/UI/Cooking/CookingSelect/dds/Leftpattern.dds", 1));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_SpikeTexture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Spike/dds/Spike%d.dds", 4));
	m_iTotalProtoCount++;

	// NPC
	m_TexturLoadingqueue.push(TEXLR(L"Proto_RatauTexture", TEX_NORMAL, L"../Bin/Resource/Texture/NPC/Ratau/dds/Ratau_%d.dds", 4));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_TarotSellerTexture", TEX_NORMAL, L"../Bin/Resource/Texture/NPC/TarotSeller/dds/TarotSeller_%d.dds", 2));
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_ChestTexture", TEX_NORMAL, L"../Bin/Resource/Texture/NPC/Chest/dds/Chest_%d.dds", 2));
	m_iTotalProtoCount++;

	// 메인보스
	vector<TEXSETLR> vecBossLeshy;
	vecBossLeshy.push_back(TEXSETLR(L"BossLeshy_Idle", L"../Bin/Resource/Texture/Monster/Boss2/idle/dds/idle_%04d.dds", 80));
	vecBossLeshy.push_back(TEXSETLR(L"BossLeshy_Dig", L"../Bin/Resource/Texture/Monster/Boss2/move-out/dds/move-out_%04d.dds", 40));
	vecBossLeshy.push_back(TEXSETLR(L"BossLeshy_Escape", L"../Bin/Resource/Texture/Monster/Boss2/move-in/dds/move-in_%04d.dds", 40));
	vecBossLeshy.push_back(TEXSETLR(L"BossLeshy_Hit", L"../Bin/Resource/Texture/Monster/Boss2/hurt/dds/hurt_%04d.dds", 16));
	vecBossLeshy.push_back(TEXSETLR(L"BossLeshy_Smash", L"../Bin/Resource/Texture/Monster/Boss2/head-smash/dds/head-smash_%04d.dds", 62));
	vecBossLeshy.push_back(TEXSETLR(L"BossLeshy_Spike", L"../Bin/Resource/Texture/Monster/Boss2/trunk-strike/dds/trunk-strike_%04d.dds", 51));
	vecBossLeshy.push_back(TEXSETLR(L"BossLeshy_Summon", L"../Bin/Resource/Texture/Monster/Boss2/summon/dds/summon_%04d.dds", 71));
	vecBossLeshy.push_back(TEXSETLR(L"BossLeshy_Spawn", L"../Bin/Resource/Texture/Monster/Boss2/transform/dds/transform_%04d.dds", 128));
	vecBossLeshy.push_back(TEXSETLR(L"BossLeshy_Die", L"../Bin/Resource/Texture/Monster/Boss2/die-noheart/dds/die-noheart_%04d.dds", 132));
	vecBossLeshy.push_back(TEXSETLR(L"BossLeshy_Dead", L"../Bin/Resource/Texture/Monster/Boss2/dead-noheart/dds/dead-noheart_%04d.dds", 1));
	m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_MonsterB2Texture", TEX_NORMAL), vecBossLeshy));
	m_iTotalProtoCount++;

	// 팔로워
	vector<TEXSETLR> vecFollower1;
	vecFollower1.push_back(TEXSETLR(L"Follower1_idle", L"../Bin/Resource/Texture/NPC/follower1/idle/dds/idle_%04d.dds", 24));
	vecFollower1.push_back(TEXSETLR(L"Follower1_run", L"../Bin/Resource/Texture/NPC/follower1/run/dds/run_%04d.dds", 14));
	vecFollower1.push_back(TEXSETLR(L"Follower1_wood", L"../Bin/Resource/Texture/NPC/follower1/chop-wood/dds/chop-wood_%04d.dds", 98));
	vecFollower1.push_back(TEXSETLR(L"Follower1_rock", L"../Bin/Resource/Texture/NPC/follower1/mining/dds/mining_%04d.dds", 46));
	vecFollower1.push_back(TEXSETLR(L"Follower1_pray", L"../Bin/Resource/Texture/NPC/follower1/pray/dds/pray_%04d.dds", 60));
	vecFollower1.push_back(TEXSETLR(L"Follower1_build", L"../Bin/Resource/Texture/NPC/follower1/build/dds/build_%04d.dds", 83));
	vecFollower1.push_back(TEXSETLR(L"Follower1_dance", L"../Bin/Resource/Texture/NPC/follower1/dance/dds/dance_%04d.dds", 47));
	vecFollower1.push_back(TEXSETLR(L"Follower1_recruit-start", L"../Bin/Resource/Texture/NPC/follower1/recruit-start/dds/recruit-start_%04d.dds", 20));
	vecFollower1.push_back(TEXSETLR(L"Follower1_recruit-loop", L"../Bin/Resource/Texture/NPC/follower1/recruit-loop/dds/recruit-loop_%04d.dds", 96));
	vecFollower1.push_back(TEXSETLR(L"Follower1_recruit-end", L"../Bin/Resource/Texture/NPC/follower1/recruit-end/dds/recruit-end_%04d.dds", 75));
	m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_Follower1Texture", TEX_NORMAL), vecFollower1));
	m_iTotalProtoCount++;

	vector<TEXSETLR> vecFollower2;
	vecFollower2.push_back(TEXSETLR(L"Follower2_idle", L"../Bin/Resource/Texture/NPC/follower2/idle/dds/idle_%04d.dds", 24));
	vecFollower2.push_back(TEXSETLR(L"Follower2_run", L"../Bin/Resource/Texture/NPC/follower2/run/dds/run_%04d.dds", 14));
	vecFollower2.push_back(TEXSETLR(L"Follower2_wood", L"../Bin/Resource/Texture/NPC/follower2/chop-wood/dds/chop-wood_%04d.dds", 98));
	vecFollower2.push_back(TEXSETLR(L"Follower2_rock", L"../Bin/Resource/Texture/NPC/follower2/mining/dds/mining_%04d.dds", 46));
	vecFollower2.push_back(TEXSETLR(L"Follower2_pray", L"../Bin/Resource/Texture/NPC/follower2/pray/dds/pray_%04d.dds", 60));
	vecFollower2.push_back(TEXSETLR(L"Follower2_build", L"../Bin/Resource/Texture/NPC/follower2/build/dds/build_%04d.dds", 83));
	vecFollower2.push_back(TEXSETLR(L"Follower2_dance", L"../Bin/Resource/Texture/NPC/follower2/dance/dds/dance_%04d.dds", 47));
	vecFollower2.push_back(TEXSETLR(L"Follower2_recruit-start", L"../Bin/Resource/Texture/NPC/follower2/recruit-start/dds/recruit-start_%04d.dds", 20));
	vecFollower2.push_back(TEXSETLR(L"Follower2_recruit-loop", L"../Bin/Resource/Texture/NPC/follower2/recruit-loop/dds/recruit-loop_%04d.dds", 96));
	vecFollower2.push_back(TEXSETLR(L"Follower2_recruit-end", L"../Bin/Resource/Texture/NPC/follower2/recruit-end/dds/recruit-end_%04d.dds", 75));
	m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_Follower2Texture", TEX_NORMAL), vecFollower2));
	m_iTotalProtoCount++;

	vector<TEXSETLR> vecFollower3;
	vecFollower3.push_back(TEXSETLR(L"Follower3_idle", L"../Bin/Resource/Texture/NPC/follower3/idle/dds/idle_%04d.dds", 24));
	vecFollower3.push_back(TEXSETLR(L"Follower3_run", L"../Bin/Resource/Texture/NPC/follower3/run/dds/run_%04d.dds", 14));
	vecFollower3.push_back(TEXSETLR(L"Follower3_wood", L"../Bin/Resource/Texture/NPC/follower3/chop-wood/dds/chop-wood_%04d.dds", 98));
	vecFollower3.push_back(TEXSETLR(L"Follower3_rock", L"../Bin/Resource/Texture/NPC/follower3/mining/dds/mining_%04d.dds", 46));
	vecFollower3.push_back(TEXSETLR(L"Follower3_pray", L"../Bin/Resource/Texture/NPC/follower3/pray/dds/pray_%04d.dds", 60));
	vecFollower3.push_back(TEXSETLR(L"Follower3_build", L"../Bin/Resource/Texture/NPC/follower3/build/dds/build_%04d.dds", 83));
	vecFollower3.push_back(TEXSETLR(L"Follower3_dance", L"../Bin/Resource/Texture/NPC/follower3/dance/dds/dance_%04d.dds", 47));
	vecFollower3.push_back(TEXSETLR(L"Follower3_recruit-start", L"../Bin/Resource/Texture/NPC/follower3/recruit-start/dds/recruit-start_%04d.dds", 20));
	vecFollower3.push_back(TEXSETLR(L"Follower3_recruit-loop", L"../Bin/Resource/Texture/NPC/follower3/recruit-loop/dds/recruit-loop_%04d.dds", 96));
	vecFollower3.push_back(TEXSETLR(L"Follower3_recruit-end", L"../Bin/Resource/Texture/NPC/follower3/recruit-end/dds/recruit-end_%04d.dds", 75));
	m_TexSetLoadingqueue.push(make_pair(TEXSETINFO(L"Proto_Follower3Texture", TEX_NORMAL), vecFollower3));
	m_iTotalProtoCount++;

}

void CMultiLoadingThread::Loading_for_TheGateway()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_TheGateway);
	m_iTotalProtoCount++;
}

void CMultiLoadingThread::Loading_for_Village()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_Village);
	m_iTotalProtoCount++;
}

void CMultiLoadingThread::Loading_for_RealDungeon()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_RealDungeon);
	m_iTotalProtoCount++;

	m_TexturLoadingqueue.push(TEXLR(L"Proto_MonsterN1Texture", TEX_NORMAL, L"../Bin/Resource/Texture/Monster/Normal1/dds/MonsterN1_%02d.dds", 7));
	m_iTotalProtoCount++;

}

void CMultiLoadingThread::Loading_for_AmdusiasRoom()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_AmdusiasRoom);
	m_iTotalProtoCount++;

}

void CMultiLoadingThread::Loading_for_LeshyRoom()
{
	m_iTotalProtoCount = 0;

	m_NonTexReadyQueue.push(&CMultiLoadingThread::NonTex_for_LeshyRoom);
	m_iTotalProtoCount++;

}

void CMultiLoadingThread::NonTex_for_Logo()
{
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcColTitle", Engine::CRcColTitle::Create(m_pGraphicDev));
}

void CMultiLoadingThread::NonTex_for_Stage()
{
}

void CMultiLoadingThread::NonTex_for_Boss()
{
}

void CMultiLoadingThread::NonTex_for_Test()
{
}

void CMultiLoadingThread::NonTex_for_KnuckleBone()
{
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
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_B1_AI", CB1_AI::Create(m_pGraphicDev, 40.f, 10.f));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_B2_AI", CB2_AI::Create(m_pGraphicDev, 40.f, 40.f));

	Load_Object_Texture(Dungeon_Texture);
}

void CMultiLoadingThread::NonTex_for_TheGateway()
{
	CPersistentMgr::GetInstance()->Ready_GlobalObjects(m_pGraphicDev);
	Load_Object_Texture(TheGateway_Texture);
}

void CMultiLoadingThread::NonTex_for_Village()
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

	Load_Object_Texture(Village_Texture);
}

void CMultiLoadingThread::NonTex_for_RealDungeon()
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
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_B1_AI", CB1_AI::Create(m_pGraphicDev, 40.f, 10.f));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_B2_AI", CB2_AI::Create(m_pGraphicDev, 40.f, 40.f));

	Load_Object_Texture(RealDungeon_Texture);
}

void CMultiLoadingThread::NonTex_for_AmdusiasRoom()
{
	//Load_Object_Texture(AmdusiasRoom_Texture);
}

void CMultiLoadingThread::NonTex_for_LeshyRoom()
{
	//Load_Object_Texture(LeshyRoom_Texture);
}

void CMultiLoadingThread::NonTex_for_Tutorial()
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
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_B1_AI", CB1_AI::Create(m_pGraphicDev, 40.f, 10.f));
	CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_B2_AI", CB2_AI::Create(m_pGraphicDev, 40.f, 40.f));

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

		std::wstring path = L"../Bin/Resource/Maps/Texture/Object/" + name + L"/dds/" + name + L"%d.dds";

		CProtoMgr::GetInstance()->Ready_Prototype(
			g_MapProtoname.back().c_str(),
			Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, path.c_str(), it->second));
	}
}

unsigned int CMultiLoadingThread::Thread_Main(void* pArg)
{
	CMultiLoadingThread* pOwner = reinterpret_cast<CMultiLoadingThread*>(pArg);

	if (!pOwner || !pOwner->m_pGraphicDev) { return 0; }

	while (!g_bDistroyWindowFlag) {
		_bool bDidWork = false;


		TaskFunc Func = nullptr;
		if (pOwner->m_NonTexReadyQueue.try_pop(Func))
		{
			if (g_bDistroyWindowFlag) break;
			if (!pOwner->m_pGraphicDev) break;
			(pOwner->*Func)();
			pOwner->m_iCompletedCount++;
			bDidWork = true;
		}

		TEXLR Texrequest;
		if (pOwner->m_TexturLoadingqueue.try_pop(Texrequest)) {

			if (g_bDistroyWindowFlag) break;
			if (!pOwner->m_pGraphicDev) break;

			CProtoMgr::GetInstance()->Ready_Prototype(Texrequest.strProtoName,
				CTexture::CreateFromThread(pOwner->m_pGraphicDev, Texrequest.eTexType, Texrequest.strFilepath, Texrequest.iTexIndex));

			pOwner->m_iCompletedCount++;
			bDidWork = true;
		}

		pair<TEXSETINFO, vector<TEXSETLR>> Pairrequest;
		if (pOwner->m_TexSetLoadingqueue.try_pop(Pairrequest)) {

			if (g_bDistroyWindowFlag) break;
			if (!pOwner->m_pGraphicDev) break;

			TEXSETINFO texsetinfo = std::move(Pairrequest.first);
			vector<TEXSETLR> vecTexsetLR = std::move(Pairrequest.second);

			CProtoMgr::GetInstance()->Ready_Prototype(texsetinfo.strProtoName,
				CTextureSet::CreateFromThered(pOwner->m_pGraphicDev, texsetinfo.eTexType, vecTexsetLR));

			pOwner->m_iCompletedCount++;
			bDidWork = true;
		}

		if (!bDidWork && pOwner->m_NonTexReadyQueue.empty() && pOwner->m_TexturLoadingqueue.empty() && pOwner->m_TexSetLoadingqueue.empty()) {
			break;
		}
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
	// 안전장치 해당플래그는 윈도우 메세지루프에서 WM_DESTROY 메세지 처리 분기에 해당 플래그 켜줌
	//g_bDistroyWindowFlag = true;
	// 아니 안돼 우리는 로딩씬 재사용하기때문에 이거 켜져있으면 다음 스레드 구동때 스레드가 일을 안함

	if (!m_vecThread.empty()) {
		WaitForMultipleObjects(WORKER_COUNT, m_vecThread.data(), TRUE, INFINITE);

		for (HANDLE& hThread : m_vecThread) {
			if (hThread) CloseHandle(hThread);
		}
		m_vecThread.clear();
	}

	Safe_Release(m_pGraphicDev);
}

