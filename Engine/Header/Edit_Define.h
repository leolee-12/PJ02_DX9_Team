#pragma once
#include <iostream>
#include <string>

/* -------------------------------------------
* 
*	1. IMGUI define 선언
	2. ImGui에서 생성하고 저장할 내용에 대해 정의

-------------------------------------------- */

// 오브젝트 이름 만들어주는 매크로
// 방법 1: 기본 매크로
#define STR_INDEX(str, index) (str + std::to_string(index))
// 방법 2: const char* 버전
#define CSTR_INDEX(str, index) (std::string(str) + std::to_string(index)).c_str()
// 방법 3: 와이드 문자열 버전 (L"...")
#define WSTR_INDEX(str, index) (std::wstring(str) + std::to_wstring(index))


// ======================
//		ObjectsToPlace
// =======================
enum OBJECT_TYPE
{
	TYPE_OBJECT,
	TYPE_LIGHT,
};

inline const char* object_type_str[] = 
{ 
	"type_object",
	"type_light",
};

// 설치할 오브젝트 enum값
enum OBJECT_ID
{
	OBJECT_TEST,
	//OBJECT_PLAYER_START,
	//OBJECT_MONSTER,
	//OBJECT_BOSSMONSTER,
	//OBJECT_PORTAL,
	//OBJECT_TREE,

	OBJECT_END
};

inline const char* object_id_str[] =
{
	"object_id_test",
};


// 이펙트 설치 (특정하게 반복되는 이펙트일 경우)
enum LIGHT_ID
{
	LIGHT_TEST,

	LIGHT_END
};

inline const char* light_id_str[] =
{
	"light_id_test",
};

// ======================
//		data for save
// =======================

// 파일 저장 방식
struct ObjectSaveData
{
	std::string objectName;
	OBJECT_TYPE objectType;
	OBJECT_ID objectID;

	float PosX;
	float PosY;
	float PosZ;

	float Size;
};

struct LightData
{

};

#define HEADER_SIZE					sizeof(SaveHeader)
#define OBJECT_DATA_SIZE			sizeof(ObjectData)
#define EFFECT_DATA_SIZE			sizeof(EffectData)
