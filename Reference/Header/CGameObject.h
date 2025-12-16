#pragma once
#include "CBase.h"
#include "CComponent.h"
#include "Engine_MsgChannel.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* SceneChannel);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject();

public:
	CComponent* Get_Component(COMPONENTID eID, const _tchar* pComponentTag);
	OBJID		Get_OBJID() { return m_eOBJID; }

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	map<const _tchar*, CComponent*>			m_mapComponent[ID_END];
	LPDIRECT3DDEVICE9						m_pGraphicDev;


private:
	CComponent* Find_Component(COMPONENTID eID, const _tchar* pComponentTag);

protected:
	virtual		void		Free();
	virtual     void		Unsubscribe_Handles();

protected:
	IMessageChannel* m_pMessageChannel;
	OBJID			 m_eOBJID;
	unordered_map<wstring, IMessageChannel::SUBHANDLE> m_hmapSubHandles;

	_int m_iHp = 1;
};

END