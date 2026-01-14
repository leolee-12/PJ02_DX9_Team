#include "pch.h"
#include "CSpeechBubble.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CSpeechBubble::CSpeechBubble(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CSpeechBubble::~CSpeechBubble()
{
}

HRESULT CSpeechBubble::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
	m_pTransformCom->Set_Scale(632 * m_fScale, 374 * m_fScale, 0.f);
	m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
    return S_OK;
}


HRESULT CSpeechBubble::Ready_Material()
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

_int CSpeechBubble::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return iExit;
}

void CSpeechBubble::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	D3DXMATRIX matView, matProj, matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXVECTOR3 vScreenPos;
	D3DVIEWPORT9 vp;
	m_pGraphicDev->GetViewport(&vp);

	D3DXVec3Project(&vScreenPos, &m_vTargetPos, &vp, &matProj, &matView, &matWorld);

	m_pTransformCom->Set_Pos(vScreenPos.x - (WINCX / 2), - vScreenPos.y + (WINCY / 2), vScreenPos.z);

	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);


	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}

void CSpeechBubble::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();
}

void CSpeechBubble::OnCollision(CGameObject* pObject)
{

}

HRESULT CSpeechBubble::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex
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

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CookingSelectSlot"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}



CSpeechBubble* CSpeechBubble::Create(LPDIRECT3DDEVICE9 pGraphicDev,_vec3 _vTargetPos,_float _fScale)
{
	CSpeechBubble* pSpeechBubble = new CSpeechBubble(pGraphicDev);

	pSpeechBubble->m_vTargetPos = _vTargetPos;
	pSpeechBubble->m_fScale = _fScale;

	if (FAILED(pSpeechBubble->Ready_GameObject()))
	{
		Safe_Release(pSpeechBubble);
		MSG_BOX("pSpeechBubble Create Failed");
		return nullptr;
	}

	return pSpeechBubble;
}

void CSpeechBubble::Free()
{
	CUi::Free();
}