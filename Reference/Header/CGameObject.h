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
	void		Set_MessageChannel(IMessageChannel* pChannel) 
	{
		if (m_pMessageChannel) {
			Safe_Release(m_pMessageChannel);
		}
		m_pMessageChannel = pChannel;
		m_pMessageChannel->AddRef();
	}
	CComponent* Get_Component(COMPONENTID eID, const wstring& strComponentTag);
	OBJID		Get_OBJID() { return m_eOBJID; }
	_float		Get_Depth() { return m_fDepth; }

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

	void			Compute_ViewDepth(const _vec3* pPos);

protected:
	map<wstring, CComponent*>				m_mapComponent[ID_END];
	LPDIRECT3DDEVICE9						m_pGraphicDev;
	
	_float									m_fDepth;

protected:
	virtual		void		Free();
	void		Unsubscribe_Handles();

	void		Requst_Unsubscribe(const wstring& strEventType);

private:
	CComponent* Find_Component(COMPONENTID eID, const wstring& strComponentTag);


protected:
	IMessageChannel* m_pMessageChannel;
	OBJID			 m_eOBJID;
	unordered_map<wstring, IMessageChannel::SUBHANDLE> m_hmapSubHandles;

	_int m_iHp = 1;
};

END