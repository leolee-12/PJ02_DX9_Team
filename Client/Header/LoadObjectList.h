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

// Tutorial.txt
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

// Dungeon.txt
set<wstring> Dungeon_Texture =
{
    L"Alter",
    L"Candle",
    L"Chain",
    L"Circle",
    L"Dirt",
    L"DungeonDoor",
    L"Gateway",
    L"Grass",
    L"Pillar",
    L"Rock",
    L"Ruin",
    L"Skull",
    L"Stairs",
    L"Stone",
    L"Stump",
    L"Tent",
    L"Tree",
    L"TutorialObject",
    L"Twig"
};

// Village.txt
set<wstring> Village_Texture =
{
    L"Circle",
    L"DungeonDoor",
    L"Gateway",
    L"Grass",
    L"Rock",
    L"Rubble",
    L"Teleport",
    L"Tree",
    L"VillageStructure"
};

// TheGateway.txt
set<wstring> TheGateway_Texture =
{
    L"Chain",
    L"Gateway",
    L"Skull",
    L"Twig"
};

// Teleport.txt
set<wstring> Teleport_Texture =
{
    L"Bush",
    L"Candle",
    L"Circle",
    L"DungeonDoor",
    L"Grass",
    L"Icon",
    L"Mushroom",
    L"Rock",
    L"Teleport",
    L"Tree",
    L"Twig",
    L"Web",
    L"Weed"
};


