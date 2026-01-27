#include "pch.h"
#include "CShrineSpot.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CInteractMgr.h"
#include "CFontMgr.h"
#include "CResourceWorkBar.h"
#include "CItem.h"
#include <CSoundMgr.h>
//#include "CPersistentMgr.h"

CShrineSpot::CShrineSpot(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	//, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_fScale(1.f)
	, m_fWorkGauge(0.f)
	, m_fPreWorkGauge(0.f)
{
}

CShrineSpot::CShrineSpot(const CShrineSpot& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	//, m_pTextureCom(nullptr)
	, m_pColliderCom(nullptr)
	, m_fScale(rhs.m_fScale)
	, m_fWorkGauge(0.f)
	, m_fPreWorkGauge(0.f)
{
}

CShrineSpot::~CShrineSpot()
{
}

HRESULT CShrineSpot::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fScale = 1.f;
	m_pTransformCom->Set_Scale(m_fScale, m_fScale, m_fScale);

	CInteractMgr::GetInstance()->Register_IObj(CInteractMgr::PRAY, this);

	m_pWorkBar = CResourceWorkBar::Create(m_pGraphicDev, MAX_WORK_GAUGE, _vec3{});
	m_pWorkBar->UnActive();

	return S_OK;
}

_int CShrineSpot::Update_GameObject(const _float& fTimeDelta)
{
	if (g_bDebug) { m_pColliderCom->Update_AABBforRender(); }

	m_fAcmlTime += fTimeDelta;

	m_pColliderCom->UpdateFromTransform(m_pTransformCom);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	Update_WorkBar(fTimeDelta);

	if (iExit == DEAD)
	{
		m_pColliderCom->UnregisterFromManager();
		CInteractMgr::GetInstance()->Unregister_IObj(CInteractMgr::PRAY, this);
	}

	//CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CShrineSpot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_fWorkGauge - m_fPreWorkGauge > 0.0001f)
	{
		m_pWorkBar->Active();

		if (m_fAcmlTime >= 1.f)
		{
			//_uint iChannel = Get_Rand_Int(SOUND_EFFECT1, SOUND_EFFECT10);

			_tchar strSoundName[128] = L"";
			swprintf_s(strSoundName, L"chanting%d.wav", Get_Rand_Int(1, 11));
			CSoundMgr::GetInstance()->Play(strSoundName, SOUND_PRAY, 0.5f);

			m_fAcmlTime = 0.f;
		}
	}

	Check_Status();

	m_pWorkBar->LateUpdate_GameObject(fTimeDelta);
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CShrineSpot::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

_vec3* CShrineSpot::Get_WorkPos(_vec3* pWorkPos) const
{
	m_pTransformCom->Get_Info(INFO_POS, pWorkPos);
	return pWorkPos;
}

void CShrineSpot::Check_Status()
{
	if (Is_WorkComplete())
	{
		m_fWorkGauge = 0.f;
		m_pWorkBar->UnActive();

		IMessageChannel::EVENT tEvent;
		tEvent.strType = L"Player.AddFaith";
		m_pMessageChannel->Publish(tEvent);
		//CPersistentMgr::GetInstance()->Get_Player()->Add_Faith(1.f);
	}

	m_fPreWorkGauge = m_fWorkGauge;
}

void CShrineSpot::OnCollision(CGameObject* pObject)
{
}

void CShrineSpot::Update_WorkBar(const _float& fTimeDelta)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	vPos.y += 3.f;
	m_pWorkBar->Set_TargetPos(vPos);
	m_pWorkBar->Update_CurWork(m_fWorkGauge);
	m_pWorkBar->Update_GameObject(fTimeDelta);
}

HRESULT CShrineSpot::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Grass Texture
	//pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
	//	(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BreakableStoneTexture"));
	//
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//
	//m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	// Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collider"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

CShrineSpot* CShrineSpot::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, IMessageChannel* pMessageChannel)
{
	CShrineSpot* pShrineSpot = new CShrineSpot(pGraphicDev);

	pShrineSpot->m_pMessageChannel = pMessageChannel;
	pShrineSpot->m_pMessageChannel->AddRef();

	if (FAILED(pShrineSpot->Ready_GameObject()))
	{
		Safe_Release(pShrineSpot);
		MSG_BOX("pShrineSpot Create Failed");
		return nullptr;
	}

	pShrineSpot->m_pTransformCom->Set_Pos(vPos.x, vPos.y - 1.5f, vPos.z);
	pShrineSpot->m_pTransformCom->Update_Component(0.f);

	return pShrineSpot;
}

void CShrineSpot::Free()
{
	Safe_Release(m_pWorkBar);
	CGameObject::Free();
}
