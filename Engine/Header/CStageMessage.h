#pragma once
#include "IMessageChannel.h"


BEGIN(Engine)

class ENGINE_DLL CStageMessage :
    public IMessageChannel
{
private:
    explicit CStageMessage();
    virtual ~CStageMessage();

public:
    // IMessageChannel을(를) 통해 상속됨
    void Free() override;
    SUBHANDLE Subscribe(const wstring& strEventType, function<void(const EVENT&)> func) override;
    void Publish(const EVENT& Event) override;
    void Unsubscribe(SUBHANDLE SubHandle) override;

public:
    static CStageMessage* Create();

private:
    typedef struct tagHandlerSlot {
        function<void(const EVENT&)> func;
        _uint uiVersion = 0;
        _bool bAlive = true;
    }HANDLERSLOT;

private:
    unordered_map<wstring, vector<HANDLERSLOT>> m_hmapHandlers;
    vector<SUBHANDLE> m_vecUnsubscribeQueue;
    _bool m_bRunning;
};


END
