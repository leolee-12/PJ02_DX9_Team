#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcColTitle : public CVIBuffer
{
protected:
	explicit CRcColTitle();
	explicit CRcColTitle(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CRcColTitle(const CRcColTitle& rhs);
	virtual ~CRcColTitle();

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();


public:
	static CRcColTitle* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
private:
	virtual		void	Free();
};

END
