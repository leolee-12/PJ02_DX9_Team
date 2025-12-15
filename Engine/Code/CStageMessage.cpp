#include "CStageMessage.h"

CStageMessage::CStageMessage()
{
	m_hmapHandlers.reserve(32);
}

CStageMessage::~CStageMessage()
{
}

void CStageMessage::Free()
{
}

IMessageChannel::SUBHANDLE CStageMessage::Subscribe(const wstring& strEventType, function<void(const EVENT&)> func)
{
	auto [iter, inserted] = m_hmapHandlers.try_emplace(strEventType);

	if (inserted) {
		iter->second.reserve(16);
	}

	_uint uiIndex = (_uint)iter->second.size();
	_uint uiVersion = 1;

	iter->second.push_back({ move(func), uiVersion, true });

	return { strEventType, uiIndex, uiVersion };
}

void CStageMessage::Publish(const EVENT& Event)
{
	auto iter = m_hmapHandlers.find(Event.strType);
	if (iter == m_hmapHandlers.end()) { return; }

	m_bRunning = true;

	for (auto& slot : iter->second)
	{
		if (slot.bAlive)
			slot.func(Event);
	}

	m_bRunning = false;

	for (const auto& Handles : m_vecUnsubscribeQueue) {
		Unsubscribe(Handles);
	}
	m_vecUnsubscribeQueue.clear();
}

void CStageMessage::Unsubscribe(SUBHANDLE SubHandle)
{
	auto iter = m_hmapHandlers.find(SubHandle.strEventType);
	if (iter == m_hmapHandlers.end()) { return; }

	auto& vec = iter->second;
	if (SubHandle.uiIndex >= vec.size()) { return; }

	auto& slot = vec[SubHandle.uiIndex];

	//슬롯이 살아있지 않거나, 슬롯의 버전이 핸들의 버전이랑 일치하지않으면 접근금지.
	if (slot.bAlive == false || slot.uiVersion != SubHandle.uiVersion) { return; }

	if (m_bRunning) {
		m_vecUnsubscribeQueue.push_back(SubHandle);
		return;
	}

	slot.bAlive = false;
	slot.uiVersion++;
}

CStageMessage* CStageMessage::Create()
{
	CStageMessage* pInstance = new CStageMessage;

	return pInstance;
}
