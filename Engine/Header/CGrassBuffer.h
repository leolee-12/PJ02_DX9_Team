#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CGrassBuffer : public CVIBuffer
{
private:
	explicit CGrassBuffer();
	explicit CGrassBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGrassBuffer(const CGrassBuffer& rhs);
	virtual ~CGrassBuffer();

public:
	virtual HRESULT Ready_Buffer() override;
	virtual void    Render_Buffer() override;

	// Vertex manipulation for grass sway effect
	// Offsets top vertices (indices 0, 1) by fOffsetX in local X direction
	void Set_TopVertexOffset(_float fOffsetX);

public:
	static CGrassBuffer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone() override;

private:
	virtual void Free() override;

private:
	// Original vertex positions for restoration
	_vec3 m_vOriginalPos[4];

	// Original normal and UV data
	_vec3 m_vOriginalNormal;
	_vec2 m_vOriginalUV[4];
};

END
