#include "pch.h"
#include "TestCube.h"

#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

TestCube::TestCube(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
    ZeroMemory(&m_vPos, sizeof(_vec3));
}

TestCube::TestCube(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
    : CGameObject(pGraphicDev, StageChannel)
{
    ZeroMemory(&m_vPos, sizeof(_vec3));
}

TestCube::TestCube(const TestCube& rhs)
    : CGameObject(rhs), m_vPos(rhs.m_vPos)
{
}

TestCube::~TestCube()
{
}

HRESULT TestCube::Ready_GameObject()
{
	//m_eOBJID = OID_TEST;
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_hmapSubHandles.insert({ L"Monster_Rotate", m_pMessageChannel->Subscribe(L"Monster.Move", [this](const IMessageChannel::EVENT& Event) {
		if (Event.eOBJID == this->Get_OBJID()) {
			m_pTransformCom->Rotation(ROT_Y, 1.f);
		}
		}) });


	return S_OK;
}

_int TestCube::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void TestCube::LateUpdate_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);

}

void TestCube::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pCubeTexCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

HRESULT TestCube::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// CubeTex
	pComponent = m_pCubeTexCom = dynamic_cast<Engine::CCubeTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	return S_OK;
}

TestCube* TestCube::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* StageChannel)
{
	TestCube* pTestCube = new TestCube(pGraphicDev, StageChannel);

	if (FAILED(pTestCube->Ready_GameObject()))
	{
		Safe_Release(pTestCube);
		MSG_BOX("pMonster Create Failed");
		return nullptr;
	}

	return pTestCube;
}

void TestCube::Free()
{
	Safe_Release(m_pCubeTexCom);

	CGameObject::Free();
}
