#pragma once
#include "CItem.h"

#define ITEM_INDEX_MAX 5
#define EQUIP_ITEM_INDEX_MAX 3
enum EquipmentItemID
{
	EID_NONE,EID_SWORD,EID_GAUNTLETS,EID_END
};
struct ItemTableData
{
	CItem::ITEMID	ID;
	wstring			Name;
	wstring			Flavor;
	wstring			Description;
};

struct EquipmentTableData
{
	EquipmentItemID	ID;
	wstring			Name;
	wstring			Flavor;
	wstring			Description;
};

static const ItemTableData ItemTable[ITEM_INDEX_MAX] =
{
	{ CItem::IG_GOLD, L"코인", L"난 돈이좋아 -집게사장-", L"자본주의에서 돈은\n 최고의 대화 수단입니다." },
	{ CItem::IG_WOOD, L"목재", L"너무 무겁다", L"건물을 지을때 사용할 수 있습니다." },
	{ CItem::IG_STONE,  L"석재", L"바위 처럼 단단하게 - 말파이트-",   L"건물을 지을때 사용할 수 있습니다." },
	{ CItem::IG_BERRY,  L"열매", L"붉은색 열매입니다 빨리먹는게 좋아보입니다",     L"요리할 때 사용할 수 있습니다." },
	{ CItem::IG_FERTILIZER,  L"똥", L"냄새가 심하게 납니다", L"아무짝에도 쓸모 없어보이지만\n 어딘가에는 쓸수있을것같습니다." },
};

static const EquipmentTableData EquipItemTable[EQUIP_ITEM_INDEX_MAX] =
{
	{ EquipmentItemID::EID_NONE, L"무기 장비되지않음",L"", L"현재 무기를 장비하지 않았습니다\n무기를 찾아 장비하세요."},
	{ EquipmentItemID::EID_SWORD, L"검",L"", L"검 입니다." },
	{ EquipmentItemID::EID_GAUNTLETS,  L"건틀릿",L"", L"건틀릿 입니다." },
};
