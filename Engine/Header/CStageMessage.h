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


private:                                        // hmap<이벤트명, vector<핸들러>> 식으로 특정 이벤트를 구독중인 대상들이 등록한 핸들러를 보관 할 것
    typedef struct tagHandlerSlot {             // vector에 핸들러만 보관하는게아니라 버전, 활성여부도 판단 하려고 vector에 집어넣기 편하게 구조체 선언함
        function<void(const EVENT&)> func;      // *핸들러.   function<void(const EVENT&)> : 매개변수로 const EVENT& 를 받고 아무것도 반환하지 않는 함수/람다/펑터를 담을 수 있는 타입
        _uint uiVersion = 0;                    // 해당하는 슬롯의 버전정보
        _bool bAlive = true;                    // 해당하는 슬롯의 활성여부
    }HANDLERSLOT;                               // *핸들러 : 특정 이벤트나 상황을 처리하는 함수 또는 객체 / 여기에서는 특정 이벤트가 발생했을때 그 이벤트를 처리하는 함수를 뜻함

private:
    unordered_map<wstring, vector<HANDLERSLOT>> m_hmapHandlers;   // 특정 이벤트의 핸들러의 컨테이너 (hmap으로 관리하는이유 - 핸들러를 저장하는데에 있어서 현재 정렬 기능이 필요없고 빠른탐색을 우선하기 때문)
    vector<SUBHANDLE> m_vecUnsubscribeQueue;                      // 구독 해제 함수가 실행되었을때 구독을 해제하면 안되는 상황일경우 해제하고자하는 핸들러의 핸들을 해당 컨테이너에 보관하는 용도
    _bool m_bRunning;                                             // 구독 해제 함수가 실행되면 안되는 상황을 나타내는 변수
};


END
