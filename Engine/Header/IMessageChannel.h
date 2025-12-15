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
	typedef struct tagEvent {
		wstring strType;
		OBJID   eOBJID;
		unordered_map<wstring, any> hmapData;
	}EVENT;

	typedef struct tagSubscriptionHandle {
		wstring strEventType;
		_uint uiIndex;		// 내가 몇번째의 인덱스에 구독을했는지 
		_uint uiVersion;	
	}SUBHANDLE;

public:
	virtual SUBHANDLE Subscribe(const wstring& strEventType, function<void(const EVENT&)> func)PURE;
	virtual void Publish(const EVENT& Event)PURE;
	virtual void Unsubscribe(SUBHANDLE SubHandle)PURE;
};

END

