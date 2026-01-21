#include "pch.h"
#include "CBishop_Heket.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontUI.h"
#include "CSpeechBubble.h"
#include "CSoundMgr.h"

CBishop_Heket::CBishop_Heket(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_iFrame(0), m_iFrameEnd(0)
	, m_eCurState(Bishops::BS_END), m_ePreState(Bishops::BS_END)
	, m_pFontUI(nullptr), m_pSpeechBubble(nullptr)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
	ZeroMemory(&m_tSpawndata, sizeof(Engine::SPAWNDATA));
}

CBishop_Heket::~CBishop_Heket()
{
}

HRESULT CBishop_Heket::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Event();

	m_pTransformCom->Set_Scale(269.f * 0.025f, 451.f * 0.025f, 0.f);

	_vec3 vPos = { m_tSpawndata.x * 0.8f, 2.f, m_tSpawndata.z * 0.8f };
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	m_eCurState = Bishops::BS_IDLE;

	vPos.y += 10.f;
	Ready_Dialogue(vPos);

	return S_OK;
}

HRESULT CBishop_Heket::Ready_Material()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

_int CBishop_Heket::Update_GameObject(const _float& fTimeDelta)
{
	Update_State();
	Update_Frame(fTimeDelta);

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	m_pFontUI->Update_GameObject(fTimeDelta);
	m_pSpeechBubble->Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CBishop_Heket::LateUpdate_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Compute_Bilboard(BBD_X);
	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);
	Compute_ViewDepth(&m_vPos);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBishop_Heket::Render_GameObject()
{
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (FAILED(Ready_Material()))
		return;

	m_pTextureCom->Set_Texture(m_strStateKey, m_iFrame);

	m_pBufferCom->Render_Buffer();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void CBishop_Heket::OnCollision(CGameObject* pObject)
{

}

HRESULT CBishop_Heket::Add_Component()
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

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTextureSet*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BishopHeket"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void		CBishop_Heket::Update_State()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Bishops::BS_IDLE:
			m_strStateKey = L"Bishop_Heket_Idle";
			m_iFrame = 0;
			m_iFrameEnd = m_pTextureCom->Get_TextureEnd(m_strStateKey);
			break;
		case Bishops::BS_TALK:
			m_strStateKey = L"Bishop_Heket_Talk";
			m_iFrame = 0;
			m_iFrameEnd = m_pTextureCom->Get_TextureEnd(m_strStateKey);
			break;
		}
		m_ePreState = m_eCurState;
	}

}

void CBishop_Heket::Update_Frame(const _float& fTimeDelta)
{
	if (m_iFrame < m_iFrameEnd)
	{
		switch (m_eCurState)
		{
		case Bishops::BS_IDLE:
			m_iFrame += 1;
			break;
		case Bishops::BS_TALK:
			m_iFrame += 1;
			break;
		}
	}
	else
	{
		m_iFrame = 0;
	}
}

void CBishop_Heket::Ready_Event()
{
	m_hmapSubHandles.insert({ L"Dialogue", m_pMessageChannel->Subscribe(L"CutScene.Dialogue", [this](const IMessageChannel::EVENT& Event)
		{
			auto TargetNameiter = Event.hmapData.find(L"TargetName");
			if (TargetNameiter == Event.hmapData.end()) { return; }
			if (any_cast<wstring>(TargetNameiter->second) != L"Bishop_Heket")
			{
				m_pSpeechBubble->UnActive();
				m_pFontUI->UnActive();
				m_eCurState = Bishops::BS_IDLE;
				return;
			}

			auto Textiter = Event.hmapData.find(L"Text");
			if (Textiter == Event.hmapData.end()) { return; }

			m_pFontUI->Set_Text(any_cast<wstring>(Textiter->second));
			m_pFontUI->Set_OwnerName(any_cast<wstring>(TargetNameiter->second));
			m_pSpeechBubble->Active();
			m_pFontUI->Active();
			m_eCurState = Bishops::BS_TALK;
			CSoundMgr::GetInstance()->Play(L"bc_heket.wav", SOUND_DIALOUGE, 0.1f);
		}
	) });

	m_hmapSubHandles.insert({ L"CutScene.End", m_pMessageChannel->Subscribe(L"CutScene.End", [this](const IMessageChannel::EVENT& Event)
		{
			auto SceneNameiter = Event.hmapData.find(L"SceneName");
			if (SceneNameiter == Event.hmapData.end()) { return; }
			if (any_cast<wstring>(SceneNameiter->second) != L"Tutorial_01")
			{
				m_pSpeechBubble->UnActive();
				m_pFontUI->UnActive();
				m_eCurState = Bishops::BS_IDLE;
				return;
			}
		}
	) });
}

HRESULT CBishop_Heket::Ready_Dialogue(const _vec3& vDialoguePos)
{
	m_pFontUI = CFontUI::Create(m_pGraphicDev, m_pMessageChannel);

	if (m_pFontUI == nullptr) { return E_FAIL; }

	m_pFontUI->Set_Font(L"Font_Lapture30");
	m_pFontUI->Set_FontColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	//DT_CENTER | DT_VCENTER
	m_pFontUI->Set_Flags(DT_CENTER | DT_VCENTER);
	m_pFontUI->Set_Scale(_vec2(500.f, 150.f));

	m_pFontUI->Set_WorldPos(vDialoguePos);

	m_pSpeechBubble = CSpeechBubble::Create(m_pGraphicDev, vDialoguePos, _vec2(500.f, 150.f));

	if (m_pSpeechBubble == nullptr) { return E_FAIL; }

	return S_OK;
}


CBishop_Heket* CBishop_Heket::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, const Engine::SPAWNDATA& tSpawndata)
{
	CBishop_Heket* pBishop_Heket = new CBishop_Heket(pGraphicDev);

	pBishop_Heket->m_tSpawndata = tSpawndata;
	pBishop_Heket->m_pMessageChannel = pMessageChannel;
	pBishop_Heket->m_pMessageChannel->AddRef();

	if (FAILED(pBishop_Heket->Ready_GameObject()))
	{
		Safe_Release(pBishop_Heket);
		MSG_BOX("pBishop_Heket Create Failed");
		return nullptr;
	}

	return pBishop_Heket;
}

void CBishop_Heket::Free()
{
	Safe_Release(m_pSpeechBubble);
	Safe_Release(m_pFontUI);
	CGameObject::Free();
}
