#include "pch.h"
#include "CPersistentMgr.h"
#include "CPlayer.h"
#include "CProtoMgr.h"

CPersistentMgr* CPersistentMgr::m_pInstance = nullptr;

CPersistentMgr::CPersistentMgr()
{
}

CPersistentMgr::~CPersistentMgr()
{
	Free();
}

HRESULT CPersistentMgr::Ready_GlobalObjects(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (m_hmapGlobalObjects.empty())
	{
		vector<CTextureSet::TEXINFO> tempVec(16);
		//tempVec[0]	= CTextureSet::TEXINFO(L"idle",				L"../Bin/Resource/Texture/Player/idle/idle_%04d.png", 72);
		//tempVec[1]	= CTextureSet::TEXINFO(L"idle-up",			L"../Bin/Resource/Texture/Player/idle-up/idle-up_%04d.png", 72);
		//tempVec[2]	= CTextureSet::TEXINFO(L"run-up",			L"../Bin/Resource/Texture/Player/run-up/run-up_%04d.png", 9);
		//tempVec[3]	= CTextureSet::TEXINFO(L"run-down",			L"../Bin/Resource/Texture/Player/run-down/run-down_%04d.png", 9);
		//tempVec[4]	= CTextureSet::TEXINFO(L"run-diagonal",		L"../Bin/Resource/Texture/Player/run/run_%04d.png", 9);
		//tempVec[5]	= CTextureSet::TEXINFO(L"run-horizontal",	L"../Bin/Resource/Texture/Player/run-horizontal/run-horizontal_%04d.png", 9);
		//tempVec[6]	= CTextureSet::TEXINFO(L"run-up-diagonal",	L"../Bin/Resource/Texture/Player/run-up-diagonal/run-up-diagonal_%04d.png", 9);
		//tempVec[7]	= CTextureSet::TEXINFO(L"roll-up",			L"../Bin/Resource/Texture/Player/roll-up/roll-up_%04d.png", 8);
		//tempVec[8]	= CTextureSet::TEXINFO(L"roll-down",		L"../Bin/Resource/Texture/Player/roll-down/roll-down_%04d.png", 8);
		//tempVec[9]	= CTextureSet::TEXINFO(L"roll-horizontal",	L"../Bin/Resource/Texture/Player/roll/roll_%04d.png", 9);
		//tempVec[10]	= CTextureSet::TEXINFO(L"attack-combo1",	L"../Bin/Resource/Texture/Player/attack-combo1/attack-combo1_%04d.png", 15);
		//tempVec[11]	= CTextureSet::TEXINFO(L"attack-combo2",	L"../Bin/Resource/Texture/Player/attack-combo2/attack-combo2_%04d.png", 19);
		//tempVec[12]	= CTextureSet::TEXINFO(L"attack-combo3",	L"../Bin/Resource/Texture/Player/attack-combo3/attack-combo3_%04d.png", 19);
		//tempVec[13]	= CTextureSet::TEXINFO(L"charge-start",		L"../Bin/Resource/Texture/Player/cast-spell2/cast-spell2_%04d.png", 16);
		//tempVec[14]	= CTextureSet::TEXINFO(L"charge-loop",		L"../Bin/Resource/Texture/Player/cast-spell2-loop/cast-spell2-loop_%04d.png", 13);
		//tempVec[15]	= CTextureSet::TEXINFO(L"charge-end",		L"../Bin/Resource/Texture/Player/cast-spell/cast-spell_%04d.png", 16);
		
		//tempVec[0]	= CTextureSet::TEXINFO(L"idle",				L"../Bin/Resource/Texture/Player(2)/idle/%04d.png", 72);
		//tempVec[1]	= CTextureSet::TEXINFO(L"idle-up",			L"../Bin/Resource/Texture/Player(2)/idle-up/%04d.png", 72);
		//tempVec[2]	= CTextureSet::TEXINFO(L"run-up",			L"../Bin/Resource/Texture/Player(2)/run-up/%04d.png", 9);
		//tempVec[3]	= CTextureSet::TEXINFO(L"run-down",			L"../Bin/Resource/Texture/Player(2)/run-down/%04d.png", 9);
		//tempVec[4]	= CTextureSet::TEXINFO(L"run-diagonal",		L"../Bin/Resource/Texture/Player(2)/run/%04d.png", 9);
		//tempVec[5]	= CTextureSet::TEXINFO(L"run-horizontal",	L"../Bin/Resource/Texture/Player(2)/run-horizontal/%04d.png", 9);
		//tempVec[6]	= CTextureSet::TEXINFO(L"run-up-diagonal",	L"../Bin/Resource/Texture/Player(2)/run-up-diagonal/%04d.png", 9);
		//tempVec[7]	= CTextureSet::TEXINFO(L"roll-up",			L"../Bin/Resource/Texture/Player(2)/roll-up/%04d.png", 8);
		//tempVec[8]	= CTextureSet::TEXINFO(L"roll-down",		L"../Bin/Resource/Texture/Player(2)/roll-down/%04d.png", 8);
		//tempVec[9]	= CTextureSet::TEXINFO(L"roll-horizontal",	L"../Bin/Resource/Texture/Player(2)/roll/%04d.png", 9);
		//tempVec[10]	= CTextureSet::TEXINFO(L"attack-combo1",	L"../Bin/Resource/Texture/Player(2)/attack-combo1/%04d.png", 15);
		//tempVec[11]	= CTextureSet::TEXINFO(L"attack-combo2",	L"../Bin/Resource/Texture/Player(2)/attack-combo2/%04d.png", 19);
		//tempVec[12]	= CTextureSet::TEXINFO(L"attack-combo3",	L"../Bin/Resource/Texture/Player(2)/attack-combo3/%04d.png", 19);
		//tempVec[13]	= CTextureSet::TEXINFO(L"charge-start",		L"../Bin/Resource/Texture/Player(2)/cast-spell2/%04d.png", 16);
		//tempVec[14]	= CTextureSet::TEXINFO(L"charge-loop",		L"../Bin/Resource/Texture/Player(2)/cast-spell2-loop/%04d.png", 13);
		//tempVec[15]	= CTextureSet::TEXINFO(L"charge-end",		L"../Bin/Resource/Texture/Player(2)/cast-spell/%04d.png", 16);

		tempVec[0]	= CTextureSet::TEXINFO(L"idle",				L"../Bin/Resource/Texture/Player(3)/idle/idle_%04d.png", 72);
		tempVec[1]	= CTextureSet::TEXINFO(L"idle-up",			L"../Bin/Resource/Texture/Player(3)/idle-up/idle-up_%04d.png", 72);
		tempVec[2]	= CTextureSet::TEXINFO(L"run-up",			L"../Bin/Resource/Texture/Player(3)/run-up/run-up_%04d.png", 9);
		tempVec[3]	= CTextureSet::TEXINFO(L"run-down",			L"../Bin/Resource/Texture/Player(3)/run-down/run-down_%04d.png", 9);
		tempVec[4]	= CTextureSet::TEXINFO(L"run-diagonal",		L"../Bin/Resource/Texture/Player(3)/run/run_%04d.png", 9);
		tempVec[5]	= CTextureSet::TEXINFO(L"run-horizontal",	L"../Bin/Resource/Texture/Player(3)/run-horizontal/run-horizontal_%04d.png", 9);
		tempVec[6]	= CTextureSet::TEXINFO(L"run-up-diagonal",	L"../Bin/Resource/Texture/Player(3)/run-up-diagonal/run-up-diagonal_%04d.png", 9);
		tempVec[7]	= CTextureSet::TEXINFO(L"roll-up",			L"../Bin/Resource/Texture/Player(3)/roll-up/roll-up_%04d.png", 8);
		tempVec[8]	= CTextureSet::TEXINFO(L"roll-down",		L"../Bin/Resource/Texture/Player(3)/roll-down/roll-down_%04d.png", 8);
		tempVec[9]	= CTextureSet::TEXINFO(L"roll-horizontal",	L"../Bin/Resource/Texture/Player(3)/roll/roll_%04d.png", 9);
		tempVec[10]	= CTextureSet::TEXINFO(L"attack-combo1",	L"../Bin/Resource/Texture/Player(3)/attack-combo1/attack-combo1_%04d.png", 15);
		tempVec[11]	= CTextureSet::TEXINFO(L"attack-combo2",	L"../Bin/Resource/Texture/Player(3)/attack-combo2/attack-combo2_%04d.png", 19);
		tempVec[12]	= CTextureSet::TEXINFO(L"attack-combo3",	L"../Bin/Resource/Texture/Player(3)/attack-combo3/attack-combo3_%04d.png", 19);
		tempVec[13]	= CTextureSet::TEXINFO(L"charge-start",		L"../Bin/Resource/Texture/Player(3)/cast-spell2/cast-spell2_%04d.png", 16);
		tempVec[14]	= CTextureSet::TEXINFO(L"charge-loop",		L"../Bin/Resource/Texture/Player(3)/cast-spell2-loop/cast-spell2-loop_%04d.png", 13);
		tempVec[15]	= CTextureSet::TEXINFO(L"charge-end",		L"../Bin/Resource/Texture/Player(3)/cast-spell/cast-spell_%04d.png", 16);

		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerTexture", Engine::CTextureSet::Create(pGraphicDev, TEX_NORMAL, tempVec))))
			return E_FAIL;

		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlayerTexture2", Engine::CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Player/Player1_%02d.png", 1))))
			return E_FAIL;

		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Collider", Engine::CCollider::Create(pGraphicDev))))
			return E_FAIL;

		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Calculator", Engine::CCalculator::Create(pGraphicDev))))
			return E_FAIL;

		CGameObject* pGameObject = CPlayer::Create(pGraphicDev);

		if (nullptr == pGameObject)
			return E_FAIL;

		m_hmapGlobalObjects[GOBJ_PLAYER] = pGameObject;
	}
	return S_OK;
}

CGameObject* CPersistentMgr::Get_GlobalObjects(GOBJID eGOBJID)
{
	auto iter = m_hmapGlobalObjects.find(eGOBJID);
	if (iter == m_hmapGlobalObjects.end()) { return nullptr; }

	return iter->second;
}

Engine::CTransform* CPersistentMgr::Get_PlayerTransform()
{
	unordered_map<GOBJID, CGameObject*>::iterator iter = m_hmapGlobalObjects.find(GOBJ_PLAYER);
	if (iter == m_hmapGlobalObjects.end()) { return nullptr; }

	return static_cast<CTransform*>(iter->second->Get_Component(ID_DYNAMIC, L"Com_Transform"));
}

void CPersistentMgr::Free()
{
	for (auto& hmap : m_hmapGlobalObjects)
	{
		m_vecReleaseQueue.push_back(hmap.second);
		hmap.second = nullptr;
	}

	for (auto& Obj : m_vecReleaseQueue)
	{
		Safe_Release(Obj);
	}
	m_vecReleaseQueue.clear();
}