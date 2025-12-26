#include "perlin_jitter.h"
#include "Engine_Define.h"

namespace Engine {

	ENGINE_DLL std::mt19937 rng(std::random_device{}());

	ENGINE_DLL PerlinJitter g_Jitter;

	int Get_Rand_Int(int min, int max) {
		std::uniform_int_distribution<int> dist(min, max);
		return dist(rng);
	}

	float Get_Rand_Float(float min, float max) {
		std::uniform_real_distribution<float> dist(min, max);
		return dist(rng);
	}

	Vec2 Get_Offset(float fTime)
	{
		return g_Jitter.get_offset(fTime);
	}
}