#pragma once
#include "CScene.h"

class CLoadingFG;
class CLoadingThread;
class CMultiLoadingThread;

class CLoading : public CScene
{
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID);
	virtual ~CLoading();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);

	void			Load_Object_Texture(const std::set<std::wstring>& textureSet);

private:
	HRESULT			Ready_Prototype();

	HRESULT			Excute_Ready_Texture();

private:
	//CLoadingThread* m_pLoading;
	CMultiLoadingThread* m_pLoading;
	LOADINGID		m_eChangeID;
	CLoadingFG*		m_pLoadingFG;

	_uint			m_iCompletedCount;
	_uint			m_iTotalCount;

	_uint           m_iLoadingFrame;
	_float			m_fLoadingDelay;

public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID ChangeID);
private:
	virtual void Free();

};

