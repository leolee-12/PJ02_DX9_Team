#pragma once
#include "CBase.h"
#include "Engine_Define.h"



BEGIN(Engine)

class ENGINE_DLL IMessageChannel : public CBase
{
protected:
	explicit IMessageChannel() {}
	virtual ~IMessageChannel() {}

public:
	typedef struct tagEvent {						// 이벤트 발생시 매개변수로 받을 구조체
		wstring strType;							// 이벤트명 문자열
		OBJID   eOBJID;								// 해당 이벤트를 적용시킬 대상의 타입으로 활용(지금은 임시임)
		unordered_map<wstring, any> hmapData;		// 이벤트 발생자가 수신자에게 넘겨줄 데이터 컨테이너 (any자료형은 아무 자료형이나 될 수 있음 [c++17])
	}EVENT;											// any자료형의 상세한 사용법은 마소래퍼런스참조 링크: https://learn.microsoft.com/ko-kr/cpp/standard-library/any-class?view=msvc-170

	typedef struct tagSubscriptionHandle { // 구독시에 반환하는 값으로 쓸 구조체 / 구독 해제시 핸들값으로 어떤 구독을 해제할지 결정
		wstring strEventType;			   // 이벤트명 문자열
		_uint uiIndex;					   // 내가 몇번째의 인덱스에 구독을했는지 
		_uint uiVersion;				   // 핸들의 인덱스 값이 겹치는 경우에 해당 핸들의 버전과 구독정보의 인덱스에 해당하는 정보의 버전을 비교하는 용도
	}SUBHANDLE;

public:
	virtual SUBHANDLE Subscribe(const wstring& strEventType, function<void(const EVENT&)> func)PURE; // 구독함수
	virtual void Publish(const EVENT& Event)PURE;													 // 이벤트 발생함수
	virtual void Unsubscribe(SUBHANDLE SubHandle)PURE;												 // 구독해제 함수

	// 해당 클래스는 인터페이스클래스이므로
	// 함수들의 내부 구현은 상속받는 구현클래스 참조
};

END

