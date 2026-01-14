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
	CComponent*		Get_Component(COMPONENTID eID, const wstring& strObjTag, const wstring& strComponentTag);
	HRESULT			Add_GameObject(const wstring& strObjTag, CGameObject* pGameObject);

public:
	HRESULT			Ready_Layer();
	_int			Update_Layer(const _float& fTimeDelta);
	void			LateUpdate_Layer(const _float& fTimeDelta);

private:
	map<wstring, vector<CGameObject*>>			m_mapObject;

public:
	static CLayer* Create();

private:
	virtual void	Free();
};

END