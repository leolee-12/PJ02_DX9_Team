#include "pch.h"
#include "CResourceHistoryController.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CResourceHistoryUI.h"

#include "CDInputMgr.h"
#include "CFontMgr.h"
#include "CItem.h"

CResourceHistoryController::CResourceHistoryController(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUi(pGraphicDev)
	, m_bRender(true)
	, m_bMsgRegistered(false)
{
	ZeroMemory(&m_vPos, sizeof(_vec3));
}

CResourceHistoryController::~CResourceHistoryController()
{
}
HRESULT CResourceHistoryController::Ready_GameObject()
{
	CResourceHistoryUI* pGameObject = nullptr;
	pGameObject = CResourceHistoryUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ WINCX / 2,WINCY / 2 - 240.0f, 0.1f }, L"동전", 0);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mResourceUI.insert({ 0 , pGameObject });

	pGameObject = CResourceHistoryUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ WINCX / 2,(WINCY / 2 - 180.0f), 0.1f }, L"나무", 1);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mResourceUI.insert({ 1 , pGameObject });
	pGameObject = CResourceHistoryUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ WINCX / 2,WINCY / 2 - 120.0f, 0.1f }, L"돌", 2);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mResourceUI.insert({ 2 , pGameObject });
	pGameObject = CResourceHistoryUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ WINCX / 2,WINCY / 2 - 60.0f, 0.1f }, L"열매", 3);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mResourceUI.insert({ 3 , pGameObject });
	pGameObject = CResourceHistoryUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ WINCX / 2,WINCY / 2, 0.1f }, L"똥", 4);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mResourceUI.insert({ 4 , pGameObject });
	pGameObject = CResourceHistoryUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ WINCX / 2,WINCY / 2 + 60.0f,0.1f }, L"빨콩", 5);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mResourceUI.insert({ 5 , pGameObject });


	pGameObject = CResourceHistoryUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ WINCX / 2,WINCY / 2 + 120.0f,0.1f }, L"밥", 6);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mResourceUI.insert({ 6 , pGameObject });


	pGameObject = CResourceHistoryUI::Create(m_pGraphicDev, m_pMessageChannel, _vec3{ WINCX / 2,WINCY / 2 + 180.0f,0.1f }, L"탄밥", 7);
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mResourceUI.insert({ 7 , pGameObject });

	for (int i = 0; i < 8; ++i)
	{
		_vec3 pos;
		pos.x = 100;
		pos.y = 230 + (50 * i);
		pos.z = 0.f;

		m_vHistoryPos.push_back(pos);
	}
	tempcount = 0;

	return S_OK;
}

_int CResourceHistoryController::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) return NOEVENT;

	// 테스트용
	//if (CDInputMgr::GetInstance()->Key_Down(DIK_X))
	//{
	//	IMessageChannel::EVENT tEvent;
	//	tEvent.strType = L"ResourceHistory.AddItem";
	//	tEvent.hmapData[L"ItemID"] = tempcount;


	//	m_pMessageChannel->Publish(tEvent);
	//	tempcount++;
	//}
	//*if (tempcount > 4) tempcount = 0;*/

	for (auto& UI : m_mResourceUI)
		UI.second->Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return NOEVENT;
}

void CResourceHistoryController::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (!m_bRender) { return; }
	for (auto UI : m_mResourceUI)
	{
		UI.second->LateUpdate_GameObject(fTimeDelta);
	}
}

void CResourceHistoryController::Render_GameObject()
{
	if (!m_bRender) { return; }
}

void CResourceHistoryController::OnCollision(CGameObject* pObject)
{

}

CResourceHistoryController* CResourceHistoryController::Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel)
{
	CResourceHistoryController* pCResourceHistoryController = new CResourceHistoryController(pGraphicDev);
	if (FAILED(pCResourceHistoryController->Ready_GameObject()))
	{
		Safe_Release(pCResourceHistoryController);
		MSG_BOX("pCResourceHistoryController Create Failed");
		return nullptr;
	}

	return pCResourceHistoryController;
}

CResourceHistoryController* CResourceHistoryController::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CResourceHistoryController* pCResourceHistoryController = new CResourceHistoryController(pGraphicDev);

	if (FAILED(pCResourceHistoryController->Ready_GameObject()))
	{
		Safe_Release(pCResourceHistoryController);
		MSG_BOX("pCResourceHistoryController Create Failed");
		return nullptr;
	}
	return pCResourceHistoryController;
}

void CResourceHistoryController::Ready_Event()
{
	if (m_pMessageChannel == nullptr) return;
	if (m_bMsgRegistered) return;

	m_hmapSubHandles.insert({ L"History.Close",m_pMessageChannel->Subscribe(L"ResourceHistory.Close",[this](const IMessageChannel::EVENT& Event)
	{
			auto iter = Event.hmapData.find(L"ItemIndex");
			if (iter == Event.hmapData.end())
				return;

			int ItemIndex = any_cast<int>(iter->second);
			Sort(ItemIndex);
	}
) });
	m_hmapSubHandles.insert({ L"History.AddItem",m_pMessageChannel->Subscribe(L"ResourceHistory.AddItem",[this](const IMessageChannel::EVENT& Event)
{
		auto iter = Event.hmapData.find(L"ItemID");
		if (iter == Event.hmapData.end())
			return;

		int ItemIndex = any_cast<int>(iter->second);
		AddItem(ItemIndex,1);
}
) });
	m_bMsgRegistered = true;
}


void CResourceHistoryController::UpdateHistory(CResourceHistoryUI* _TargetUI, RESOURCEHISTORYUISTATE _estate, _int _iCount, _vec3 _vStart, _vec3 _vEnd,_int _Order)
{
	if (!_TargetUI) return;

	switch (_estate)
	{
	case STATE_NEW:
	{
		// 새로운 히스토리
		_TargetUI->Reset_DisPlayTime();
		_TargetUI->Reset_DeltaAmount();
		_TargetUI->Apply_TotalCount(_iCount);
		_TargetUI->Apply_DeltaAmount(_iCount);
		_TargetUI->Moveto(_vStart, _vEnd);
		_TargetUI->SetOrder(_Order);
		_TargetUI->Set_bClose(false);
		break;
	}
	case STATE_UPDATE:
	{
		// 띄워졌던 히스토리
		_TargetUI->Reset_DisPlayTime();
		_TargetUI->Apply_TotalCount(_iCount);
		_TargetUI->Apply_DeltaAmount(_iCount);
		_TargetUI->Set_bClose(false);
		break;
	}
	case STATE_REORDER:
	{
		// 정렬이후 움직임만 적용
		_TargetUI->Moveto(_vStart, _vEnd);
		_TargetUI->SetOrder(_Order);
		break;
	}
	case STATE_END:
		break;
	default:
		break;
	}
}

void CResourceHistoryController::Sort(_int _index)
{
	auto it = find(m_vHistoryData.begin(), m_vHistoryData.end(), _index);

	if (it == m_vHistoryData.end())
		return;

	m_vHistoryData.erase(it);

	ReorderHistory();
}

void CResourceHistoryController::ReorderHistory()
{
	for (int i = 0; i < m_vHistoryData.size(); ++i)
	{
		int index = m_vHistoryData[i];

		auto iter = m_mResourceUI.find(index);
		if (iter == m_mResourceUI.end())
			continue;

		CResourceHistoryUI* pUI = iter->second;

		UpdateHistory(pUI, STATE_REORDER, 0, pUI->Get_Pos(), m_vHistoryPos[i],i);
	}
}

void CResourceHistoryController::AddItem(_int _iItemIndex, _int _iCount)
{
	auto iter = m_mResourceUI.find(_iItemIndex);
	if (iter == m_mResourceUI.end())
		return;

	CResourceHistoryUI* pUI = iter->second;

	if (!pUI)
		return;


	auto it = std::find(m_vHistoryData.begin(), m_vHistoryData.end(), _iItemIndex);
	if (it != m_vHistoryData.end())
	{
		// UPDATE
		UpdateHistory(pUI, STATE_UPDATE, _iCount, pUI->Get_Pos(), m_vHistoryPos[m_vHistoryData.size()],pUI->GetOrder());
	}
	else
	{
		// NEW
		_vec3 EndPos = m_vHistoryPos[m_vHistoryData.size()];
		_vec3 startPos = _vec3{ EndPos.x - 300.0f,EndPos.y,EndPos.z };
		UpdateHistory(pUI, STATE_NEW, _iCount, startPos, EndPos, m_vHistoryData.size());
		m_vHistoryData.push_back(_iItemIndex);
	}
}
void CResourceHistoryController::Set_MessageChannel(IMessageChannel* pMessageChannel)
{

	if (pMessageChannel == nullptr) { return; }
	if (m_pMessageChannel) {
		CGameObject::Unsubscribe_Handles();
		Safe_Release(m_pMessageChannel);
		m_bMsgRegistered = false;
	}
	m_pMessageChannel = pMessageChannel;
	m_pMessageChannel->AddRef();
	for (auto it : m_mResourceUI)
	{
		it.second->Set_MessageChannel(pMessageChannel);
	}
	Ready_Event();
}


void CResourceHistoryController::UseItem(_int _iItemIndex, _int _iCount)
{

}

void CResourceHistoryController::Free()
{
	for (auto UI : m_mResourceUI)
	{
		Safe_Release(UI.second);
	}
	m_mResourceUI.clear();

	CUi::Free();
}
