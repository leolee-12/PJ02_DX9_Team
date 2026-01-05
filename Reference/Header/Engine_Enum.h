#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum WINMODE { MODE_FULL, MODE_WIN };

	// Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
	enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	enum INFO {	INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };

	enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };
	
	enum BILBOARD { BBD_X, BBD_Y, BBD_Z, BBD_END};

	enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

	enum RENDERID { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };

	enum MOUSEMOVESTATE {	DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum OBJID { OID_PLAYER, OID_MONSTER, OID_ITEM, OID_WEAPON, OID_DCAM, OID_MAINCAM, OID_END };

	enum LOADINGID { LOADING_STAGE, LOADING_BOSS, LOADING_TEST, LOADING_KNUCKLEBONE, LOADING_END };

	enum CHANNELID { SOUND_BGM, SOUND_EFFECT, MAXCHANNEL };

	typedef enum GLOBALOBJECTID
	{
		GOBJ_PLAYER, GOBJ_END
	}GOBJID;

	enum COLGROUP
	{
		CL_NONE		= 0,
		CL_PLAYER	= 1 << 0,
		CL_MONSTER	= 1 << 1,
		CL_ITEM		= 1 << 2,
		CL_4		= 1 << 3,
		CL_5		= 1 << 4,
		CL_6		= 1 << 5,
		CL_7		= 1 << 6,
		CL_8		= 1 << 7,
		CL_9		= 1 << 8,
		CL_10		= 1 << 9,
		CL_11		= 1 << 10,
		CL_12		= 1 << 11,
		CL_13		= 1 << 12,
		CL_14		= 1 << 13,
		CL_15		= 1 << 14,
		CL_16		= 1 << 15,

		CL_ALL		= 0xFFFF // 1111 1111 1111 1111 16비트
	};
	inline COLGROUP operator|(COLGROUP lhs, COLGROUP rhs) {
		return COLGROUP((unsigned int)lhs | (unsigned int)rhs);
	}

	inline COLGROUP operator&(COLGROUP lhs, COLGROUP rhs) {
		return COLGROUP((unsigned int)lhs & (unsigned int)rhs);
	}

	enum DIRECTIONID { DIR_RIGHT, DIR_RD, DIR_DOWN, DIR_LD, DIR_LEFT, DIR_LU, DIR_UP, DIR_RU, DIR_END };
}
#endif // Engine_Enum_h__
