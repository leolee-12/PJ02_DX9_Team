#pragma once

#include "CBase.h"
#include "CFrame.h"

BEGIN(Engine)

class ENGINE_DLL CFrameMgr : public CBase
{
	DECLARE_SINGLETON(CFrameMgr)

private:
	explicit CFrameMgr();
	virtual ~CFrameMgr();

public:
	_bool	IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);
	HRESULT Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit);

	void	Update_FPS(const _float& fTimeDelta);
	_int	Get_FPS() const { return m_iFPS; }
	_float	Get_FrameTime() const { return m_fFrameTime; }

private:
	CFrame* Find_Frame(const _tchar* pFrameTag);

private:
	map<const _tchar*, CFrame*>	m_mapFrame;
	_float	m_fAccTime;
	_float	m_fFrameTime;
	_int	m_iFrameCount;
	_int	m_iFPS;

public:
	virtual void	Free();
};

END
