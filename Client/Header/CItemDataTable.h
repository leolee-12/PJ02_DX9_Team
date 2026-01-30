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
	{ CItem::IG_GOLD, L"동전", L"황금을 향한 열망은 종종 모든 것을 빼앗아가기 마련이다.", L"온갖 유용한 일에 쓰이는 돈입니다." },
	{ CItem::IG_WOOD, L"나무", L"이 통나무의 핏줄을 따라 부드러운 푸른 빛이 맥동한다", L"건축 재료로 사용되는 단순한 재료입니다." },
	{ CItem::IG_STONE,  L"돌", L"고대의 대지가 부서진 파편",   L"유용한 건축용 자재입니다." },
	{ CItem::IG_BERRY,  L"베리", L"단순하며, 과육 넘치는 자연의 산물이다",     L"요리용으로 쓰이는 맛있는 재료입니다." },
	{ CItem::IG_FERTILIZER,  L"퇴비", L"냄새가 좋지 않다... 아직 따뜻하다", L"영양소가 풍부하며, 밭 경작지에 유용합니다." },
};

static const EquipmentTableData EquipItemTable[EQUIP_ITEM_INDEX_MAX] =
{
	{ EquipmentItemID::EID_NONE, L"무기 장비되지않음",L"", L"현재 무기를 장비하지 않았습니다\n무기를 찾아 장비하세요."},
	{ EquipmentItemID::EID_SWORD, L"성기사의 검",L"", L"방랑하는 전사에게 있어 든든한 동반자이다." },
	{ EquipmentItemID::EID_GAUNTLETS,  L"템페스트의 건틀릿",L"", L"그 누구도 벗어날 수 없는 강철의 주먹" },
};
