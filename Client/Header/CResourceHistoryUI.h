#pragma once
#include "CUi.h"
#include "CResourceHistoryEnum.h"

class CResourceHistoryBack;
class CResourceHistoryDeco;
class CResourceHistoryItem;
class CFontUIOrtho;

class CResourceHistoryUI :
	public CUi
{
private:
	explicit CResourceHistoryUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CResourceHistoryUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();
	virtual			void		OnCollision(CGameObject* pObject);

public:
	static CResourceHistoryUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, IMessageChannel* pMessageChannel, _vec3 _vPos, const wstring& szName, _int _iIndex);


	void Set_Render(_bool _bRender) { m_bRender = _bRender; }
	void Set_State(RESOURCEHISTORYUISTATE eState) { m_eCurState = eState; }
	RESOURCEHISTORYUISTATE Get_State() { return m_eCurState; }
	_vec3 Get_Pos() { return m_vPos; }
	_vec3 Get_LerpEndPos() { return m_vEndLerpPos; }
	_float Get_totalCount() { return m_iTotalCount; }
	void Apply_TotalCount(_int _count) { m_iTotalCount += _count; }
	_float Get_DisplayTime() { return m_fDisplayTime; }
	void Reset_DisPlayTime() { m_fDisplayTime = m_fDisplayTimeMax; }
	void Reset_DeltaAmount() { m_iDeltaAmount = 0;}
	void Apply_DeltaAmount(_int _iDeltaAmount) { m_iDeltaAmount += _iDeltaAmount; }
	void SetOrder(_int _Order) { m_iOrder = _Order; }
	_int GetOrder() { return m_iOrder; }
	void Moveto(const _vec3& vStart, const _vec3& vEnd)
	{
		m_vStartLerpPos = vStart;
		m_vEndLerpPos = vEnd;
		m_fLerpTime = 0.f;
	}     
	void Set_bClose(_bool _b) { m_bClosing = _b; }

	float Lerp(float _fStart, float _fEnd, float t) { return _fStart + (_fEnd - _fStart) * t; }

	void   Set_MessageChannel(IMessageChannel* pMessageChannel);

private:
	virtual			void		Free();

private:
	RESOURCEHISTORYUISTATE		m_eCurState;
	RESOURCEHISTORYUISTATE		m_ePrevState;

	CResourceHistoryBack* m_pResourceHistoryBack;
	CResourceHistoryDeco* m_pResourceHistoryDeco;
	CResourceHistoryItem* m_pResourceHistoryItem;
	CFontUIOrtho*		  m_pDeltaAmountFont;
	CFontUIOrtho*		  m_pNameFont;
	CFontUIOrtho*		  m_pTotalCountFont;

	_bool		m_bRender = false;
	_bool		m_bisGet;

	_vec3		m_vStartLerpPos;
	_vec3		m_vEndLerpPos;
	_float		m_fLerpTime;
	_float		m_EndLerpTime;
	_float		m_fDisplayTime;
	_float		m_fDisplayTimeMax;

	_int		m_iIndex;
	_int		m_iCount;
	_int		m_iTotalCount;
	_int		m_iDeltaAmount;
	_int		m_iOrder;

	wstring		m_szName;

	_bool		m_bClosing;
	_vec3		m_vPos;
};
