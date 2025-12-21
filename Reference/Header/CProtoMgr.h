#pragma once

#include "CTriCol.h"
#include "CRcCol.h"
#include "CRcTex.h"
#include "CTerrainTex.h"
#include "CCubeTex.h"

#include "CTransform.h"
#include "CTexture.h"
#include "CTextureSet.h"
#include "CCalculator.h"
#include "CTerrainWallTex.h"
#include "CCollider.h"

BEGIN(Engine)

class ENGINE_DLL CProtoMgr :  public CBase
{
	DECLARE_SINGLETON(CProtoMgr)

private:
	CProtoMgr();
	virtual ~CProtoMgr();

public:
	HRESULT			Ready_Prototype(const _tchar* pComponentTag, CComponent* pComponent);
	CComponent* Clone_Prototype(const _tchar* pComponentTag);

private:
	CComponent* Find_Prototype(const _tchar* pComponentTag);

private:
	map<const _tchar*, CComponent*>		m_mapPrototype;

private:
	virtual void Free();

};

END
