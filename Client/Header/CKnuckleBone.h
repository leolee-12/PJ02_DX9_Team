#pragma once
#include "CScene.h"


class CKnuckleBone : public CScene
{
private:
	explicit CKnuckleBone(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CKnuckleBone();

public:
	virtual			HRESULT		Ready_Scene();
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

private:
	HRESULT			Ready_Environment_Layer(const _tchar* pLayerTag);
	HRESULT			Ready_GameLogic_Layer(const _tchar* pLayerTag) { return S_OK; }
	HRESULT			Ready_UI_Layer(const _tchar* pLayerTag);

	HRESULT			Ready_Light();

public:
	static CKnuckleBone* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();
};

