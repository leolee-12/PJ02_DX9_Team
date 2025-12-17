#pragma once
#include "CComponent.h"

BEGIN(Engine)

class CTerrainTex;
class CTransform;

class ENGINE_DLL CBilboard : public CComponent
{
private:
	explicit CBilboard(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBilboard(const CBilboard& rhs);
	virtual ~CBilboard();

public:
	HRESULT		Ready_Bilboard();

	

public:
	static CBilboard* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();


};
END

