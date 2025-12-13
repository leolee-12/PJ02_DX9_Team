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
    void Subscribe(const wstring& strEventType, function<void(const EVENT&)> fcHandler) override;
    void Publish(const EVENT& Event) override;

public:
    static CStageMessage* Create();


private:
    unordered_map<wstring, vector<function<void(const EVENT&)>>> hmapHandlers;
};


END
