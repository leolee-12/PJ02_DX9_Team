#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagVertexColor
	{
		_vec3		vPosition;			
		_ulong		dwColor;
	
	}VTXCOL;

	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertexTexture
	{
		_vec3		vPosition;
		_vec3		vNormal;
		_vec2		vTexUV;

	}VTXTEX;

	const _ulong	FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	typedef struct tagVertexCubeTexture
	{
		_vec3		vPosition;
		_vec3		vTexUV;

	}VTXCUBE;

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0); // �ؽ�ó�� UV ��ǥ ���� FLOAT�� 3���� ǥ���ϰڴٴ� ��ũ��(��ȣ���� ���� 0�� �ǹ̴� ���� ���ؽ��� �ؽ��� UV���� �������� �� �� �ִµ� ���� 0��° ���� �����ϰڴٴ� �ǹ�)

	typedef struct tagIndex16
	{
		_ushort  _0;
		_ushort  _1;
		_ushort  _2;

	}INDEX16;

	typedef struct tagIndex32
	{
		_ulong	_0;
		_ulong	_1;
		_ulong	_2;

	}INDEX32;	

	typedef struct tagAABB
	{
		_float x, y, z;
		_float hx, hy, hz;
	}AABB;

	// Map Data Structures
	typedef struct tagTileData
	{
		_int x, z;
		_int textureId;
		_int maskFlags;
	}TILEDATA;

	typedef struct tagSpawnData
	{
		_int type;			// 0=Player, 1=Monster
		_int monsterType;
		_float x, z;
	}SPAWNDATA;

	typedef struct tagObjectData
	{
		std::string category;
		_int textureIndex;
		_float x, y, z;
		_float scale;
		_int placement;		// 0=Standing(Billboard), 1=Floor
	}OBJECTDATA;

	typedef struct tagMapData
	{
		_int width, height;
		std::vector<TILEDATA> tiles;
		std::vector<SPAWNDATA> spawns;
		std::vector<OBJECTDATA> objects;
		_int skyType;
	}MAPDATA;

	// Mask Flags
	enum EMaskFlags
	{
		MASK_NONE = 0,
		MASK_DOWN = 1,
		MASK_LEFT = 2,
		MASK_RIGHT = 4,
		MASK_UP = 8
	};

}


#endif // Engine_Struct_h__
