#include "pch.h"
#include "CFade.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CPersistentMgr.h"


CFade::CFade(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CFade::~CFade()
{
}

HRESULT CFade::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_PixelShader();

	Ready_Event();

	m_pTransformCom->Set_Scale(_float(WINCX), _float(WINCY), 0.f);

	m_pTransformCom->Set_Pos(0.f, 0.f, 0.01f);

	return S_OK;
}

_int CFade::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);


	if (m_bActive) {
		Update_Alpha();
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}
	

	return iExit;
}

void CFade::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth_Ortho(&m_vPos);
}

void CFade::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetPixelShader(m_pPixelShader);
	m_pGraphicDev->SetPixelShaderConstantF(0, &m_fAlpha, 1);

	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetPixelShader(NULL);
}

void CFade::OnCollision(CGameObject* pObject)
{

}

HRESULT CFade::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Fade"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CFade::Update_Alpha()
{
	if (m_bActive) {
		if (m_bFadeIn)
		{
			if (m_fAlpha > 0.f)
			{
				m_fAlpha -= m_fFadeSpeed;
			}
			else
			{
				UnActive();
			}
		}
		else {
			if (m_fAlpha < 1.f)
			{
				m_fAlpha += m_fFadeSpeed;
			}
		}
	}

	m_fAlpha = clamp(m_fAlpha, 0.f, 1.f);
}

HRESULT CFade::Ready_PixelShader()
{
	LPD3DXBUFFER pCode = NULL;
	LPD3DXBUFFER pError = NULL;

	// HLSL 파일 컴파일 
	HRESULT hr = D3DXCompileShaderFromFile(
		L"../Shader/Fade.hlsl", // 파일명 
		NULL, // 매크로 
		NULL, // include 
		"PS_Fade", // 엔트리 포인트 
		"ps_2_0", // 셰이더 모델 
		0, // 플래그 
		&pCode,
		&pError,
		NULL);

	if (FAILED(hr))
	{
		if (pError)
		{
			MessageBoxA(NULL,
				(char*)pError->GetBufferPointer(),
				"Shader Error",
				MB_OK);
			pError->Release();
		}
		return E_FAIL;
	} // 픽셀 셰이더 생성 

	if (pCode) {
		m_pGraphicDev->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &m_pPixelShader);
		pCode->Release();
	}

	if (pError) {
		pError->Release();
	}

	return S_OK;
}

void	CFade::Ready_Event()
{
	m_hmapSubHandles.insert({ L"FadeCutScene", m_pMessageChannel->Subscribe(L"CutScene.Dialogue", [this](const IMessageChannel::EVENT& Event)
		{
			auto CinemaTargetNameiter = Event.hmapData.find(L"CinemaTargetName");
			if (CinemaTargetNameiter == Event.hmapData.end()) { return; }

			wstring strName = any_cast<wstring>(CinemaTargetNameiter->second);
			if (strName == L"FadeIn")
			{
				Set_FadeIn();
				m_fFadeSpeed = 0.01f;
				Active();
			}
			if (strName == L"FadeOut")
			{
				Set_FadeOut();
				m_fFadeSpeed = 0.01f;
				Active();
			}
		}
	) });

	m_hmapSubHandles.insert({ L"FadeObject", m_pMessageChannel->Subscribe(L"Fade.Warp", [this](const IMessageChannel::EVENT& Event)
		{
			auto iter = Event.hmapData.find(L"Fade");
			if (iter == Event.hmapData.end()) { return; }

			wstring strName = any_cast<wstring>(iter->second);
			if (strName == L"FadeIn")
			{
				Set_FadeIn();
				m_fFadeSpeed = 0.025f;
				Active();
			}
			if (strName == L"FadeOut")
			{
				Set_FadeOut();
				m_fFadeSpeed = 0.1f;
				Active();
			}
		}
	) });

}

void	CFade::Active()
{
	m_bActive = true;
	CRenderer::GetInstance()->Set_RenderFont(false);
	CPersistentMgr::GetInstance()->Set_GaugeFontRender(false);
}

void	CFade::UnActive()
{
	m_bActive = false;
	CRenderer::GetInstance()->Set_RenderFont(true);
	CPersistentMgr::GetInstance()->Set_GaugeFontRender(true);
}


CFade* CFade::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CFade* pFade = new CFade(pGraphicDev);

	pFade->m_pMessageChannel = pMessageChannel;
	pFade->m_pMessageChannel->AddRef();

	if (FAILED(pFade->Ready_GameObject()))
	{
		Safe_Release(pFade);
		MSG_BOX("pFade Create Failed");
		return nullptr;
	}
	return pFade;
}

void CFade::Free()
{
	m_pPixelShader->Release();
	CUi::Free();
}
