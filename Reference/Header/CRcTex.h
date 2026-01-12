#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcTex : public CVIBuffer
{
protected:
	explicit CRcTex();
	explicit CRcTex(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CRcTex(const CRcTex& rhs);
	virtual ~CRcTex();

public:
	HRESULT		Ready_Buffer(DWORD dwGridSize);
	virtual		void		Render_Buffer();


public:
	static CRcTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, DWORD dwGridSize = 2);
	virtual CComponent* Clone();
private:
	virtual		void	Free();
};

END
