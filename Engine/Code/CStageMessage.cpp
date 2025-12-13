#include "CStageMessage.h"

CStageMessage::CStageMessage()
{
	hmapHandlers.reserve(32);
}

CStageMessage::~CStageMessage()
{
}

void CStageMessage::Free()
{
}

void CStageMessage::Subscribe(const wstring& strEventType, function<void(const EVENT&)> fcHandler)
{
	auto [iter, inserted] = hmapHandlers.try_emplace(strEventType);

	if (inserted) {
		iter->second.reserve(16);
	}

	iter->second.push_back(fcHandler);
}

void CStageMessage::Publish(const EVENT& Event)
{
	auto iter = hmapHandlers.find(Event.strType);

	if (iter != hmapHandlers.end()) {
		auto CopyHandlers = iter->second;
		for (auto& functor : CopyHandlers) 
		{
			functor(Event);
		}
	}
}

CStageMessage* CStageMessage::Create()
{
	CStageMessage* pStageMessage = new CStageMessage;

	if (pStageMessage == nullptr) {
		MSG_BOX("StageMessage Create Failed");
		return nullptr;
	}

	return pStageMessage;
}
