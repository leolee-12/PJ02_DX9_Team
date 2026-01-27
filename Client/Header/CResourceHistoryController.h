#pragma once
#include "CUi.h"
#include "CResourceHistoryEnum.h"

namespace Engine
{
	class CTransform;
	class CRcTex;
	class CTexture;
}
class CResourceHistoryUI;

class CResourceHistoryController :
	public CUi
{
private:
	explicit CResourceHistoryController(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CResourceHistoryController();

public:
	virtual            HRESULT        Ready_GameObject();
	virtual            _int        Update_GameObject(const _float& fTimeDelta);
	virtual            void        LateUpdate_GameObject(const _float& fTimeDelta);
	virtual            void        Render_GameObject();
	virtual            void        OnCollision(CGameObject* pObject);

public:
	static CResourceHistoryController* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel);
	static CResourceHistoryController* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	void   Set_MessageChannel(IMessageChannel* pMessageChannel);

	void Ready_Event();
	void Set_Render(_bool _bRender) { m_bRender = _bRender; }

	float Lerp(float _fStart, float _fEnd, float t) { return _fStart + (_fEnd - _fStart) * t; }

	void UpdateHistory(CResourceHistoryUI* _TargetUI, RESOURCEHISTORYUISTATE _estate, _int _iCount, _vec3 _vStart, _vec3 _vEnd,_int _Order);
	void Sort(_int _index);
	void ReorderHistory();
	void AddItem(_int _iItemIndex, _int _iCount);
	void UseItem(_int _iItemIndex, _int _iCount);
	_int GetIndexHistoryItemCount(_int _iIndex);
private:
	virtual            void        Free();

private:
	std::map<int, CResourceHistoryUI*> m_mResourceUI;
	vector<int> m_vHistoryData;
	_bool        m_bRender;

	vector<_vec3> m_vHistoryPos;
	_bool m_bMsgRegistered;
};
