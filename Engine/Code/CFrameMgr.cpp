#include "CFrameMgr.h"

IMPLEMENT_SINGLETON(CFrameMgr)

CFrameMgr::CFrameMgr()
	: m_fAccTime(0.f)
	, m_fFrameTime(0.f)
	, m_iFrameCount(0.f)
	, m_iFPS(0)
{
}

CFrameMgr::~CFrameMgr()
{
	Free();
}

_bool CFrameMgr::IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta)
{
	CFrame* pFrame = Find_Frame(pFrameTag);
	if (nullptr == pFrame)
		return false;

	return pFrame->Ispermit_Call(fTimeDelta);
}

HRESULT CFrameMgr::Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit)
{
	CFrame* pFrame = Find_Frame(pFrameTag);

	if (nullptr != pFrame)
		return E_FAIL;

	pFrame = CFrame::Create(fCallLimit);
	if (nullptr == pFrame)
		return E_FAIL;

	m_mapFrame.insert({ pFrameTag, pFrame });

	return S_OK;
}

CFrame* CFrameMgr::Find_Frame(const _tchar* pFrameTag)
{
	auto iter = find_if(m_mapFrame.begin(), m_mapFrame.end(), CTag_Finder(pFrameTag));

	if (iter == m_mapFrame.end())
		return nullptr;

	return iter->second;
}

void CFrameMgr::Update_FPS(const _float& fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	m_iFrameCount++;

	// 1초마다 FPS 갱신
	if (m_fAccTime >= 1.f)
	{
		m_iFPS = m_iFrameCount;
		m_fFrameTime = (m_fAccTime / m_iFrameCount) * 1000.f;  // ms 단위

		m_fAccTime = 0.f;
		m_iFrameCount = 0;
	}
}

void CFrameMgr::Free()
{
	for_each(m_mapFrame.begin(), m_mapFrame.end(), CDeleteMap());
	m_mapFrame.clear();
}
