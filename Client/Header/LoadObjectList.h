#pragma once

#include <set>
#include <map>
#include <list>

using namespace std;
extern list<wstring>		g_MapProtoname;

// 전체 오브젝트 리스트
map<wstring, int> ObjectList =
{
	{L"Alter", 2},
	{L"Berry", 2},
	{L"Bush", 5},
	{L"Candle", 10},
	{L"Chain", 8},
	{L"Circle", 3},
	{L"Crate", 1},
	{L"Dirt", 3},
	{L"DungeonDoor", 15},
	{L"ExtraTile", 2},
	{L"Fire", 2},
	{L"Gateway", 6},
	{L"Grass", 21},
	{L"Icon", 5},
	{L"Mushroom", 5},
	{L"Pillar", 4},
	{L"Pole", 1},
	{L"Rock", 44},
	{L"Rubble", 18},
	{L"Ruin", 10},
	{L"Skull", 8},
	{L"Stairs", 3},
	{L"Stone", 15},
	{L"Stump", 2},
	{L"Symbol", 4},
	{L"Tarot", 15},
	{L"Teleport", 9},
	{L"Tent", 4},
	{L"Tree", 10},
	{L"TutorialObject", 6},
	{L"Twig", 4},
	{L"VillageStructure", 22},
	{L"WeaponPodium", 4},
	{L"Web", 15},
	{L"Weed", 10},
	{L"Wood", 0},
};

set<wstring> Tutorial_Texture =
{
	L"Candle",
	L"Gateway",
	L"Grass",
	L"Pillar",
	L"Rock",
	L"Skull",
	L"Stairs",
	L"Stone",
	L"Tree",
	L"TutorialObject"
};

