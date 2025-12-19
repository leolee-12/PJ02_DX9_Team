#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "IMessageChannel.h"

BEGIN(Engine)

class ENGINE_DLL CStaticChannelMgr : public CBase
{
	DECLARE_SINGLETON(CStaticChannelMgr)

private:
	explicit CStaticChannelMgr();
	virtual ~CStaticChannelMgr();

public:
	HRESULT Ready_StaticChannel();

private:
	IMessageChannel* m_pCollisionChannel;

private:
	virtual void		Free();
};

END
