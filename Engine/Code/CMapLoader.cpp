#include "CMapLoader.h"
#include <fstream>
#include <sstream>

USING(Engine)

IMPLEMENT_SINGLETON(CMapLoader)

const _float CMapLoader::TILE_SIZE = 10.0f;

CMapLoader::CMapLoader()
{
}

CMapLoader::~CMapLoader()
{
}

HRESULT CMapLoader::LoadMap(const _tchar* pFilePath, MAPDATA& outMap)
{
	char szPath[MAX_PATH] = "";
	WideCharToMultiByte(CP_ACP, 0, pFilePath, -1, szPath, MAX_PATH, NULL, NULL);
	return LoadMapA(szPath, outMap);
}

HRESULT CMapLoader::LoadMapA(const char* pFilePath, MAPDATA& outMap)
{
	std::ifstream file(pFilePath);
	if (!file.is_open())
	{
		MSG_BOX("Failed to open map file");
		return E_FAIL;
	}

	// Initialize
	outMap.width = 0;
	outMap.height = 0;
	outMap.skyType = 0;
	outMap.tiles.clear();
	outMap.spawns.clear();
	outMap.objects.clear();

	std::string line;
	std::string currentSection;

	while (std::getline(file, line))
	{
		// Skip empty lines
		if (line.empty())
			continue;

		// Check section header
		if (line[0] == '[')
		{
			currentSection = line;
			continue;
		}

		// Parse by section
		ParseSection(currentSection, line, outMap);
	}

	file.close();
	return S_OK;
}

void CMapLoader::ParseSection(const std::string& section, const std::string& line, MAPDATA& outMap)
{
	if (section == "[Map]")
	{
		sscanf_s(line.c_str(), "%d,%d", &outMap.width, &outMap.height);
	}
	else if (section == "[Tiles]")
	{
		TILEDATA tile;
		tile.maskFlags = 0;
		int count = sscanf_s(line.c_str(), "%d,%d,%d,%d",
			&tile.x, &tile.z, &tile.textureId, &tile.maskFlags);
		if (count >= 3)
		{
			outMap.tiles.push_back(tile);
		}
	}
	else if (section == "[Spawns]")
	{
		SPAWNDATA spawn;
		spawn.monsterType = 0;
		int count = sscanf_s(line.c_str(), "%d,%d,%f,%f",
			&spawn.type, &spawn.monsterType, &spawn.x, &spawn.z);
		if (count >= 3)
		{
			// Handle old format (Type,X,Z)
			if (count == 3)
			{
				spawn.x = static_cast<_float>(spawn.monsterType);
				sscanf_s(line.c_str(), "%d,%f,%f", &spawn.type, &spawn.x, &spawn.z);
				spawn.monsterType = 0;
			}
			outMap.spawns.push_back(spawn);
		}
	}
	else if (section == "[Objects]")
	{
		OBJECTDATA obj;
		char category[64] = "";
		obj.y = 0.f;
		obj.placement = 0;

		int count = sscanf_s(line.c_str(), "%[^,],%d,%f,%f,%f,%f,%d",
			category, (unsigned)_countof(category),
			&obj.textureIndex,
			&obj.x, &obj.y, &obj.z, &obj.scale, &obj.placement);

		if (count >= 5)
		{
			obj.category = category;
			outMap.objects.push_back(obj);
		}
	}
	else if (section == "[Sky]")
	{
		sscanf_s(line.c_str(), "%d", &outMap.skyType);
	}
}

void CMapLoader::Free()
{
}
