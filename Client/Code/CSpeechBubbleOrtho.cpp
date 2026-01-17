#include "pch.h"
#include "CSpeechBubbleOrtho.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CSpeechBubbleOrtho::CSpeechBubbleOrtho(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
	, m_bActive(false)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CSpeechBubbleOrtho::~CSpeechBubbleOrtho()
{
}

HRESULT CSpeechBubbleOrtho::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	m_pTransformCom->Set_Scale(m_vScale.x, m_vScale.y, 0.f);
	m_pTransformCom->Set_Pos(m_vCenterPos.x, m_vCenterPos.y, 0.1f);
	return S_OK;
}


HRESULT CSpeechBubbleOrtho::Ready_Material()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

_int CSpeechBubbleOrtho::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bActive)
	{
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CSpeechBubbleOrtho::LateUpdate_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CSpeechBubbleOrtho::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CSpeechBubbleOrtho::OnCollision(CGameObject* pObject)
{

}

HRESULT CSpeechBubbleOrtho::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTexUI"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SpeechBubble"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CSpeechBubbleOrtho* CSpeechBubbleOrtho::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 _vPos, _vec2 _vScale)
{
	CSpeechBubbleOrtho* pSpeechBubble = new CSpeechBubbleOrtho(pGraphicDev);

	pSpeechBubble->m_vCenterPos = _vPos;
	pSpeechBubble->m_vScale = _vScale;

	if (FAILED(pSpeechBubble->Ready_GameObject()))
	{
		Safe_Release(pSpeechBubble);
		MSG_BOX("pSpeechBubbleOrtho Create Failed");
		return nullptr;
	}

	return pSpeechBubble;
}

void CSpeechBubbleOrtho::Free()
{
	CUi::Free();
}
