#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"
#include "Engine_Enum.h"

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

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0); // 텍스처의 UV 좌표 값을 FLOAT형 3개로 표현하겠다는 매크로(괄호안의 숫자 0의 의미는 본래 버텍스에 텍스쳐 UV값이 여러개가 올 수 있는데 그중 0번째 값을 지정하겠다는 의미)

	typedef struct DebugVertex 
	{ 
		D3DXVECTOR3 vPosition;
		DWORD dwColor; 

	}VTXDEBUG; 

    const _ulong	FVF_DEBUG = D3DFVF_XYZ | D3DFVF_DIFFUSE;

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
        _int type;                      // 0=Player, 1=Monster
        _int monsterType;
        _float x, z;
    }SPAWNDATA;

    typedef struct tagObjectData
    {
        std::string category;
        _int textureIndex;
        _float x, y, z;
        _float scale;
        _int placement;         // 0=Standing(Billboard), 1=Floor
    }OBJECTDATA;

    typedef struct tagLightData
    {
        _float x, y, z;
        _int intensity;     // 0=Strong, 1=Medium, 2=Weak
    }LIGHTDATA;


	// Map Warp Data (same map teleport, paired by PairId)
	typedef struct tagMapWarpData
	{
		_int pairId;
		_float x, z;
		_int direction;     // 0=Left, 1=Right, 2=Up, 3=Down
	}MAPWARPDATA;

	// Scene Warp Data (different scene teleport)
	typedef struct tagSceneWarpData
	{
		_float x, z;
		_int direction;
		std::string destinationScene;
	}SCENEWARPDATA;

	typedef struct tagCollisionData
	{
		_int x, z;  // Tile coordinates
	}COLLISIONDATA;

    typedef struct tagMapData
    {
        _int width, height;
        std::vector<TILEDATA> tiles;
        std::vector<SPAWNDATA> spawns;
        std::vector<OBJECTDATA> objects;
        _int skyType;
        std::vector<LIGHTDATA> lights;
		std::vector<MAPWARPDATA> mapWarps;    
		std::vector<SCENEWARPDATA> sceneWarps;
		std::vector<COLLISIONDATA> collisions;
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

	typedef struct tagProtoLoadRequest
	{
		std::wstring strProtoName;
		std::wstring strFilepath;
		_int iTexIndex;

		TEXTUREID eTexType;

		std::vector<std::vector<BYTE>> vecTexBuffer;

		tagProtoLoadRequest() {}
		tagProtoLoadRequest(const std::wstring& _strProtoName, const TEXTUREID _eTexType, const std::wstring& _strFilepath, const _int _iTexIndex)
		{
			strProtoName = _strProtoName;
			strFilepath = _strFilepath;
			iTexIndex = _iTexIndex;
			eTexType = _eTexType;
		}
	}TEXLR;

	typedef struct tagProtoLoadRequestTexSet
	{
		std::wstring strStateName;
		std::wstring strFilepath;
		_int	iTexIndex;

		std::vector<std::vector<BYTE>> vecTexBuffer;

		tagProtoLoadRequestTexSet() {}
		tagProtoLoadRequestTexSet(const std::wstring& _strStateName, const std::wstring& _strFilepath, const _int _iTexIndex)
		{
			strStateName = _strStateName;
			strFilepath = _strFilepath;
			iTexIndex = _iTexIndex;
		}
	}TEXSETLR;

	typedef struct tagTexSetLoadInfo
	{
		std::wstring strProtoName;

		TEXTUREID eTexType;

		tagTexSetLoadInfo() {}
		tagTexSetLoadInfo(const std::wstring& _strProtoName, const TEXTUREID _eTexType)
		{
			strProtoName = _strProtoName;
			eTexType = _eTexType;
		}
	}TEXSETINFO;

}


#endif // Engine_Struct_h__
