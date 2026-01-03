#include "pch.h"
#include "CGrass.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"

CGrass::CGrass(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_iTextureIndex(0)
	, m_fScale(1.f)
	, m_fBaseScale(1.f)
	, m_fSwayAngle(0.f)
	, m_fSwaySpeed(5.f)
	, m_bIsSwaying(false)
{
}

CGrass::CGrass(const CGrass& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_iTextureIndex(rhs.m_iTextureIndex)
	, m_fScale(rhs.m_fScale)
	, m_fBaseScale(rhs.m_fBaseScale)
	, m_fSwayAngle(0.f)
	, m_fSwaySpeed(5.f)
	, m_bIsSwaying(false)
{
}

CGrass::~CGrass()
{
}

HRESULT CGrass::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CGrass::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	React_ToPlayer(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CGrass::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CGrass::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Z버퍼 쓰기 비활성화 (알파 오브젝트 앞뒤 문제 해결)
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// 알파 블렌딩 활성화
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// 알파 테스트 (완전 투명 픽셀 제거)
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x10);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->Set_Texture(m_iTextureIndex);
	m_pBufferCom->Render_Buffer();

	// 설정 복원
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CGrass::Set_ObjectData(const Engine::OBJECTDATA& objData)
{
	m_iTextureIndex = objData.textureIndex;
	m_fScale = objData.scale;
	m_fBaseScale = objData.scale;

	m_pTransformCom->Set_Pos(objData.x, objData.y, objData.z);
	m_pTransformCom->Set_Scale(m_fScale, m_fScale, m_fScale);
}

void CGrass::React_ToPlayer(const _float& fTimeDelta)
{
	// Get player transform
	Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>(
		Engine::CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

	if (nullptr == pPlayerTransform)
		return;

	_vec3 vPlayerPos;
	pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	// Calculate distance to player
	_vec3 vDist = vPlayerPos - vPos;
	vDist.y = 0.f;
	_float fDist = D3DXVec3Length(&vDist);

	const _float fReactDistance = 2.f;

	if (fDist < fReactDistance)
	{
		// Start swaying when player is nearby
		m_bIsSwaying = true;

		// Calculate sway based on player direction
		D3DXVec3Normalize(&vDist, &vDist);
		m_fSwayAngle = sinf(m_fSwaySpeed * fTimeDelta) * D3DXToRadian(15.f) * (1.f - fDist / fReactDistance);

		// Slightly compress the grass
		m_fScale = m_fBaseScale * (0.8f + 0.2f * (fDist / fReactDistance));
	}
	else if (m_bIsSwaying)
	{
		// Gradually return to normal
		m_fSwayAngle *= 0.9f;
		m_fScale += (m_fBaseScale - m_fScale) * 0.1f;

		if (fabsf(m_fSwayAngle) < 0.001f && fabsf(m_fScale - m_fBaseScale) < 0.001f)
		{
			m_bIsSwaying = false;
			m_fSwayAngle = 0.f;
			m_fScale = m_fBaseScale;
		}
	}
}

HRESULT CGrass::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// RcTex Buffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Grass Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_GrassTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CGrass* CGrass::Create(LPDIRECT3DDEVICE9 pGraphicDev, const Engine::OBJECTDATA& objData)
{
	CGrass* pGrass = new CGrass(pGraphicDev);

	if (FAILED(pGrass->Ready_GameObject()))
	{
		Safe_Release(pGrass);
		MSG_BOX("CGrass Create Failed");
		return nullptr;
	}

	pGrass->Set_ObjectData(objData);

	return pGrass;
}

void CGrass::Free()
{
	// m_pBufferCom, m_pTransformCom, m_pTextureCom are in m_mapComponent
	// They will be released by CGameObject::Free()
	CGameObject::Free();
}
