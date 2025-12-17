#include "CStageMessage.h"

CStageMessage::CStageMessage()
	: m_bRunning(false)
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
	// [c++17] pair<iterator, bool> try_emplace(키값) : 특정키가 없으면 true를 반환하고 새요소를 삽입, 키가 이미 존재하면 false를 반환하고 아무런 행동X
	// [c++17] structured binding 문법 : 여러 값을 반환하는 객체를 (pair,구조체등) 분해해서 각각의 요소에 바인딩할 수 있는 문법

	if (inserted) {					// 키값이 없어서 새 요소 삽입을 실행했다면
		iter->second.reserve(16);	// 해당 요소(벡터)의 카파시티의 크기를 16으로 할당해라
	}

	_uint uiIndex = (_uint)iter->second.size(); // 반환할 핸들내부의 인덱스 변수설정
	_uint uiVersion = 1;						// 반환할 핸들내부, 보관할 핸들러슬롯 구조체의 버전 설정

	iter->second.push_back({ move(func), uiVersion, true }); // 핸들러 삽입

	return { strEventType, uiIndex, uiVersion };			// 해당 핸들러의 핸들 반환.
}

void CStageMessage::Publish(const EVENT& Event)
{
	auto iter = m_hmapHandlers.find(Event.strType);
	if (iter == m_hmapHandlers.end()) { return; }

	m_bRunning = true;					// 이벤트 발생 함수에서 목록순회를 시작함을 알림

	for (auto& slot : iter->second)		// 해당하는 핸들러슬롯 목록을 순회
	{
		if (slot.bAlive)				// 해당하는 핸들러슬롯이 활성화되어있으면
			slot.func(Event);			// 저장되어있는 핸들러를 실행
	}									// *이 과정에서 해당 핸들러 내부에 구독 해제 함수가 실행되면 순회중에 삭제가 이뤄지기때문에 문제발생
										// 그걸 막을 수 있게 도와주는게 m_bRunning 변수
	m_bRunning = false;					// 이벤트 발생 함수에서 목록순회를 종료함을 알림

	for (const auto& Handles : m_vecUnsubscribeQueue) { // 목록 순회가 종료된 뒤에 구독 해제를 해야할 핸들러의 핸들보관 컨테이너를 순회
		Unsubscribe(Handles);							// 순회하는 요소(핸들)으로 구독 해제 함수 실행
	}
	m_vecUnsubscribeQueue.clear();						// 조건없이 컨테이너에 존재하는 모든 핸들을 구독해제했으니 내부 요소들을 모두 삭제함.
}

void CStageMessage::Unsubscribe(SUBHANDLE SubHandle)
{
	auto iter = m_hmapHandlers.find(SubHandle.strEventType);
	if (iter == m_hmapHandlers.end()) { return; }

	auto& vec = iter->second;
	if (SubHandle.uiIndex >= vec.size()) { return; }		// 인덱스가 요소의 개수보다 크거나 같으면 범위초과(사이즈가 10이면 해당 벡터의 마지막 인덱스는 9이기 때문)

	auto& slot = vec[SubHandle.uiIndex];					// 해당 인덱스를 통해서 벡터의 요소에 접근 (핸들러슬롯)

	//슬롯이 살아있지 않거나, 슬롯의 버전이 핸들의 버전이랑 일치하지않으면 접근금지.
	if (slot.bAlive == false || slot.uiVersion != SubHandle.uiVersion) { return; }

	if (m_bRunning) {										// 이벤트 발생 함수에서 목록순회중이라면.
		m_vecUnsubscribeQueue.push_back(SubHandle);			// 지연처리 컨테이너에 해당하는 핸들을 저장하고 리턴함.
		return;
	}

	slot.bAlive = false;									// 비활성화된 슬롯으로 전환
	slot.uiVersion++;										// 버전 갱신
}

CStageMessage* CStageMessage::Create()
{
	CStageMessage* pInstance = new CStageMessage;

	return pInstance;
}
