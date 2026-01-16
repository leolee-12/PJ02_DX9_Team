#include "pch.h"
#include "CPersistentMgr.h"
#include "CProtoMgr.h"

CPersistentMgr* CPersistentMgr::m_pInstance = nullptr;

CPersistentMgr::CPersistentMgr()
	: m_pPlayer(nullptr)
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
		tempVec[0]	= CTextureSet::TEXINFO(L"idle",				L"../Bin/Resource/Texture/Player/idle/idle_%04d.png", 72);
		tempVec[1]	= CTextureSet::TEXINFO(L"idle-up",			L"../Bin/Resource/Texture/Player/idle-up/idle-up_%04d.png", 72);
		tempVec[2]	= CTextureSet::TEXINFO(L"run-up",			L"../Bin/Resource/Texture/Player/run-up/run-up_%04d.png", 9);
		tempVec[3]	= CTextureSet::TEXINFO(L"run-down",			L"../Bin/Resource/Texture/Player/run-down/run-down_%04d.png", 9);
		tempVec[4]	= CTextureSet::TEXINFO(L"run-diagonal",		L"../Bin/Resource/Texture/Player/run/run_%04d.png", 9);
		tempVec[5]	= CTextureSet::TEXINFO(L"run-horizontal",	L"../Bin/Resource/Texture/Player/run-horizontal/run-horizontal_%04d.png", 9);
		tempVec[6]	= CTextureSet::TEXINFO(L"run-up-diagonal",	L"../Bin/Resource/Texture/Player/run-up-diagonal/run-up-diagonal_%04d.png", 9);
		tempVec[7]	= CTextureSet::TEXINFO(L"roll-up",			L"../Bin/Resource/Texture/Player/roll-up/roll-up_%04d.png", 8);
		tempVec[8]	= CTextureSet::TEXINFO(L"roll-down",		L"../Bin/Resource/Texture/Player/roll-down/roll-down_%04d.png", 8);
		tempVec[9]	= CTextureSet::TEXINFO(L"roll-horizontal",	L"../Bin/Resource/Texture/Player/roll/roll_%04d.png", 9);
		tempVec[10]	= CTextureSet::TEXINFO(L"attack-combo1",	L"../Bin/Resource/Texture/Player/attack-combo1/attack-combo1_%04d.png", 15);
		tempVec[11]	= CTextureSet::TEXINFO(L"attack-combo2",	L"../Bin/Resource/Texture/Player/attack-combo2/attack-combo2_%04d.png", 19);
		tempVec[12]	= CTextureSet::TEXINFO(L"attack-combo3",	L"../Bin/Resource/Texture/Player/attack-combo3/attack-combo3_%04d.png", 19);
		tempVec[13]	= CTextureSet::TEXINFO(L"charge-start",		L"../Bin/Resource/Texture/Player/cast-spell2/cast-spell2_%04d.png", 16);
		tempVec[14]	= CTextureSet::TEXINFO(L"charge-loop",		L"../Bin/Resource/Texture/Player/cast-spell2-loop/cast-spell2-loop_%04d.png", 13);
		tempVec[15]	= CTextureSet::TEXINFO(L"charge-end",		L"../Bin/Resource/Texture/Player/cast-spell/cast-spell_%04d.png", 16);
		tempVec[16] = CTextureSet::TEXINFO(L"knockback",		L"../Bin/Resource/Texture/Player/knockback/knockback_%04d.png", 20);

		// 인트로용
		tempVec[17] = CTextureSet::TEXINFO(L"intro_idle",				L"../Bin/Resource/LWY/Player/Player_intro/intro_idle/intro_idle_%04d.png", 36);
		tempVec[18] = CTextureSet::TEXINFO(L"intro_idle-up",			L"../Bin/Resource/LWY/Player/Player_intro/intro_idle-up/intro_idle-up_%04d.png", 36);
		tempVec[19] = CTextureSet::TEXINFO(L"intro_run-up",				L"../Bin/Resource/LWY/Player/Player_intro/intro_run-up/intro_run-up_%04d.png", 24);
		tempVec[20] = CTextureSet::TEXINFO(L"intro_run-down",			L"../Bin/Resource/LWY/Player/Player_intro/intro_run-down/intro_run-down_%04d.png", 24);
		tempVec[21] = CTextureSet::TEXINFO(L"intro_run-diagonal",		L"../Bin/Resource/LWY/Player/Player_intro/intro_run/intro_run_%04d.png", 24);
		tempVec[22] = CTextureSet::TEXINFO(L"intro_run-horizontal",		L"../Bin/Resource/LWY/Player/Player_intro/intro_run-horizontal/intro_run-horizontal_%04d.png", 24);
		tempVec[23] = CTextureSet::TEXINFO(L"intro_run-up-diagonal",	L"../Bin/Resource/LWY/Player/Player_intro/intro_run-up-diagonal/intro_run-up-diagonal_%04d.png", 24);

		tempVec[24] = CTextureSet::TEXINFO(L"intro_kneel",		L"../Bin/Resource/LWY/Player/Player_intro/intro_kneel/intro_kneel_%04d.png", 27);
		tempVec[25] = CTextureSet::TEXINFO(L"intro_kneel-loop", L"../Bin/Resource/LWY/Player/Player_intro/intro_kneel-loop/intro_kneel-loop_%04d.png", 16);
		tempVec[26] = CTextureSet::TEXINFO(L"intro_kneel-wake", L"../Bin/Resource/LWY/Player/Player_intro/intro_kneel-wake/intro_kneel-wake_%04d.png", 108);
		tempVec[27] = CTextureSet::TEXINFO(L"intro_rebirth",	L"../Bin/Resource/LWY/Player/Player_intro/intro_rebirth/intro_rebirth_%04d.png", 164);

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
	return S_OK;
}

Engine::CTransform* CPersistentMgr::Get_PlayerTransform()
{
	return static_cast<CTransform*>(m_pPlayer->Get_Component(ID_DYNAMIC, L"Com_Transform"));
}



void CPersistentMgr::Free()
{
	Safe_Release(m_pPlayer);
}
