#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcTexXZ : public CVIBuffer
{
protected:
	explicit CRcTexXZ();
	explicit CRcTexXZ(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CRcTexXZ(const CRcTexXZ& rhs);
	virtual ~CRcTexXZ();

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();


public:
	static CRcTexXZ* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
private:
	virtual		void	Free();
};

END
