#include "pch.h"
#include "CPersistentMgr.h"
#include "CProtoMgr.h"

CPersistentMgr* CPersistentMgr::m_pInstance = nullptr;

CPersistentMgr::CPersistentMgr()
	: m_pPlayer(nullptr), m_pGauge(nullptr), m_pPlayerHPUI(nullptr), m_pResourceHistoryUI(nullptr)
{
}

CPersistentMgr::~CPersistentMgr()
{
	Free();
}

HRESULT CPersistentMgr::Ready_GlobalObjects(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (m_pPlayer == nullptr)
	{
		vector<CTextureSet::TEXINFO> tempVec(28);
		tempVec[0] = CTextureSet::TEXINFO(L"idle", L"../Bin/Resource/Texture/Player/idle/dds/idle_%04d.dds", 72);
		tempVec[1] = CTextureSet::TEXINFO(L"idle-up", L"../Bin/Resource/Texture/Player/idle-up/dds/idle-up_%04d.dds", 72);
		tempVec[2] = CTextureSet::TEXINFO(L"run-up", L"../Bin/Resource/Texture/Player/run-up/dds/run-up_%04d.dds", 9);
		tempVec[3] = CTextureSet::TEXINFO(L"run-down", L"../Bin/Resource/Texture/Player/run-down/dds/run-down_%04d.dds", 9);
		tempVec[4] = CTextureSet::TEXINFO(L"run-diagonal", L"../Bin/Resource/Texture/Player/run/dds/run_%04d.dds", 9);
		tempVec[5] = CTextureSet::TEXINFO(L"run-horizontal", L"../Bin/Resource/Texture/Player/run-horizontal/dds/run-horizontal_%04d.dds", 9);
		tempVec[6] = CTextureSet::TEXINFO(L"run-up-diagonal", L"../Bin/Resource/Texture/Player/run-up-diagonal/dds/run-up-diagonal_%04d.dds", 9);
		tempVec[7] = CTextureSet::TEXINFO(L"roll-up", L"../Bin/Resource/Texture/Player/roll-up/dds/roll-up_%04d.dds", 8);
		tempVec[8] = CTextureSet::TEXINFO(L"roll-down", L"../Bin/Resource/Texture/Player/roll-down/dds/roll-down_%04d.dds", 8);
		tempVec[9] = CTextureSet::TEXINFO(L"roll-horizontal", L"../Bin/Resource/Texture/Player/roll/dds/roll_%04d.dds", 9);
		tempVec[10] = CTextureSet::TEXINFO(L"attack-combo1", L"../Bin/Resource/Texture/Player/attack-combo1/dds/attack-combo1_%04d.dds", 15);
		tempVec[11] = CTextureSet::TEXINFO(L"attack-combo2", L"../Bin/Resource/Texture/Player/attack-combo2/dds/attack-combo2_%04d.dds", 19);
		tempVec[12] = CTextureSet::TEXINFO(L"attack-combo3", L"../Bin/Resource/Texture/Player/attack-combo3/dds/attack-combo3_%04d.dds", 19);
		tempVec[13] = CTextureSet::TEXINFO(L"charge-start", L"../Bin/Resource/Texture/Player/cast-spell2/dds/cast-spell2_%04d.dds", 16);
		tempVec[14] = CTextureSet::TEXINFO(L"charge-loop", L"../Bin/Resource/Texture/Player/cast-spell2-loop/dds/cast-spell2-loop_%04d.dds", 13);
		tempVec[15] = CTextureSet::TEXINFO(L"charge-end", L"../Bin/Resource/Texture/Player/cast-spell/dds/cast-spell_%04d.dds", 16);
		tempVec[16] = CTextureSet::TEXINFO(L"knockback", L"../Bin/Resource/Texture/Player/knockback/dds/knockback_%04d.dds", 20);
		// 인트로용
		tempVec[17] = CTextureSet::TEXINFO(L"intro_idle", L"../Bin/Resource/LWY/Player/Player_intro/intro_idle/dds/intro_idle_%04d.dds", 36);
		tempVec[18] = CTextureSet::TEXINFO(L"intro_idle-up", L"../Bin/Resource/LWY/Player/Player_intro/intro_idle-up/dds/intro_idle-up_%04d.dds", 36);
		tempVec[19] = CTextureSet::TEXINFO(L"intro_run-up", L"../Bin/Resource/LWY/Player/Player_intro/intro_run-up/dds/intro_run-up_%04d.dds", 24);
		tempVec[20] = CTextureSet::TEXINFO(L"intro_run-down", L"../Bin/Resource/LWY/Player/Player_intro/intro_run-down/dds/intro_run-down_%04d.dds", 24);
		tempVec[21] = CTextureSet::TEXINFO(L"intro_run-diagonal", L"../Bin/Resource/LWY/Player/Player_intro/intro_run/dds/intro_run_%04d.dds", 24);
		tempVec[22] = CTextureSet::TEXINFO(L"intro_run-horizontal", L"../Bin/Resource/LWY/Player/Player_intro/intro_run-horizontal/dds/intro_run-horizontal_%04d.dds", 24);
		tempVec[23] = CTextureSet::TEXINFO(L"intro_run-up-diagonal", L"../Bin/Resource/LWY/Player/Player_intro/intro_run-up-diagonal/dds/intro_run-up-diagonal_%04d.dds", 24);
		tempVec[24] = CTextureSet::TEXINFO(L"intro_kneel", L"../Bin/Resource/LWY/Player/Player_intro/intro_kneel/dds/intro_kneel_%04d.dds", 27);
		tempVec[25] = CTextureSet::TEXINFO(L"intro_kneel-loop", L"../Bin/Resource/LWY/Player/Player_intro/intro_kneel-loop/dds/intro_kneel-loop_%04d.dds", 16);
		tempVec[26] = CTextureSet::TEXINFO(L"intro_kneel-wake", L"../Bin/Resource/LWY/Player/Player_intro/intro_kneel-wake/dds/intro_kneel-wake_%04d.dds", 108);
		tempVec[27] = CTextureSet::TEXINFO(L"intro_rebirth", L"../Bin/Resource/LWY/Player/Player_intro/intro_rebirth/dds/intro_rebirth_%04d.dds", 164);

		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerTexture", Engine::CTextureSet::Create(pGraphicDev, TEX_NORMAL, tempVec))))
			return E_FAIL;

		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Collider", Engine::CCollider::Create(pGraphicDev))))
			return E_FAIL;

		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Calculator", Engine::CCalculator::Create(pGraphicDev))))
			return E_FAIL;

		m_pPlayer = CPlayer::Create(pGraphicDev);

		if (nullptr == m_pPlayer)
			return E_FAIL;
	}

	if (m_pGauge == nullptr)
	{
		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GaugeCover", Engine::CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/dds/GaugeCover.dds", 1))))
			return E_FAIL;

		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_GaugeStar", Engine::CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/dds/GaugeStar.dds", 1))))
			return E_FAIL;

		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PassionGauge", Engine::CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/dds/PassionGauge.dds", 1))))
			return E_FAIL;

		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PassionIcon", Engine::CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/PassionGauge/dds/PassionIcon_%d.dds", 2))))
			return E_FAIL;

		m_pGauge = CGauge::Create(pGraphicDev, Gauge::GS_PASSION);
	}

	if (m_pPlayerHPUI == nullptr)
	{
		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerHP", Engine::CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/Player/dds/Health_%d.dds", 3))))
			return E_FAIL;

		m_pPlayerHPUI = CPlayerHP::Create(pGraphicDev);
	}

	if (m_pResourceHistoryUI == nullptr)
	{
		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ItemHistoryBack", Engine::CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/YSD/ResourceHistory/ItemHistoryBack_%d.png", 2))))
			return E_FAIL;
		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ItemHistoryDeco", Engine::CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/YSD/ResourceHistory/ItemHistoryDeco.png", 1))))
			return E_FAIL;
		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ItemHistoryItem", Engine::CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Item/dds/Item_%02d.dds", 8))))
			return E_FAIL;

		m_pResourceHistoryUI = CResourceHistoryController::Create(pGraphicDev);
	}
	return S_OK;
}

void CPersistentMgr::Update_PersistnetMgr(const _float fTimeDelta)
{
	Update_PlayerHp();
	Update_PlayerGage();
}

Engine::CTransform* CPersistentMgr::Get_PlayerTransform()
{
	return static_cast<CTransform*>(m_pPlayer->Get_Component(ID_DYNAMIC, L"Com_Transform"));
}

void CPersistentMgr::Set_GaugeFontRender(_bool bBool)
{
	m_pGauge->Set_FontRender(bBool);
}

void CPersistentMgr::Update_PlayerHp()
{
	m_pPlayerHPUI->Set_MaxHp(m_pPlayer->Get_MaxHp());
	m_pPlayerHPUI->Set_Hp(m_pPlayer->Get_Hp());
}

void CPersistentMgr::Update_PlayerGage()
{
}

void CPersistentMgr::Free()
{
	Safe_Release(m_pPlayer);
	Safe_Release(m_pGauge);
	Safe_Release(m_pPlayerHPUI);
	Safe_Release(m_pVillage);
	Safe_Release(m_pResourceHistoryUI);
}
