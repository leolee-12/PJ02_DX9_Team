#pragma once
#include "CBase.h"
#include "CGameObject.h"
#include "Engine_MsgChannel.h"

BEGIN(Engine)

class ENGINE_DLL CLayer : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer();

public:
	CComponent*		Get_Component(COMPONENTID eID, const _tchar* pObjTag, const _tchar* pComponentTag);
	HRESULT			Add_GameObject(const _tchar* pObjTag, CGameObject* pGameObject);

public:
	HRESULT			Ready_Layer();
	_int			Update_Layer(const _float& fTimeDelta);
	void			LateUpdate_Layer(const _float& fTimeDelta);

private:
	map<const _tchar*, vector<CGameObject*>>			m_mapObject;

public:
	static CLayer* Create();

private:
	virtual void	Free();
};

END