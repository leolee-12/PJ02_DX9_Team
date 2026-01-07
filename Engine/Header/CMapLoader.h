#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

/* ===================================================
	CMapLoader

	�� ������ �ִ� txt ������ �ҷ����� �Ŵ���
 =====================================================*/

class ENGINE_DLL CMapLoader : public CBase
{
	DECLARE_SINGLETON(CMapLoader)

private:
	explicit CMapLoader();
	virtual ~CMapLoader();

public:
	HRESULT LoadMap(const _tchar* pFilePath, MAPDATA& outMap);
	HRESULT LoadMapA(const char* pFilePath, MAPDATA& outMap);

private:
	void ParseSection(const std::string& section, const std::string& line, MAPDATA& outMap);

public:
	static const _float TILE_SIZE;

private:
	virtual void Free() override;
};

END
