#pragma once



/* -------------------------------------------
* 
*	1. IMGUI define 선언
	2. ImGui에서 생성하고 저장할 내용에 대해 정의

-------------------------------------------- */

// ======================
//		ObjectsToPlace
// =======================
enum IMGUI_TYPE
{
	IMGUITYPE_OBJECT,
	IMGUITYPE_EFFECT,
};

// 설치할 오브젝트 enum값
enum IMGUI_OBJECT
{
	PLAYER_START,
	//MONSTER,
	//BOSSMONSTER,
	PORTAL,
	TREE,
};

// 이펙트 설치 (특정하게 반복되는 이펙트일 경우)
enum IMGUI_EFFECT
{
	EFFECT,

};

// ======================
//		data for save
// =======================

// 파일 저장 방식
struct SaveHeader
{
	int Index;
	IMGUI_TYPE type;
};

struct ObjectData
{
	IMGUI_OBJECT Object;

	float PosX;
	float PosY;
	float PosZ;

	float Size;
};

struct EffectData
{

};

#define HEADER_SIZE					sizeof(SaveHeader)
#define OBJECT_DATA_SIZE			sizeof(ObjectData)
#define EFFECT_DATA_SIZE			sizeof(EffectData)
