#pragma once
#include "CBase.h"
#include "Engine_Define.h"



BEGIN(Engine)

class ENGINE_DLL IMessageChannel : public CBase
{
public:
	explicit IMessageChannel() {}
	virtual ~IMessageChannel() {}

public:
	typedef struct tagEvent
	{
		wstring strType;
		OBJID   eOBJID;
		unordered_map<wstring, any> hmapData;

		tagEvent(const wstring& strEventType)
			: strType(strEventType), eOBJID(OID_END) {}
	}EVENT;

public:
	virtual void Subscribe(const wstring& strEventType, function<void(const EVENT&)> fcHandler)PURE;
	virtual void Publish(const EVENT& Event)PURE;
};

END

