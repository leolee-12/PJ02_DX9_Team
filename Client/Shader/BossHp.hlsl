float LerpUV;
float CutUV;

sampler2D texSampler : register(s0);

float4 PS_BossHp(float2 uv : TEXCOORD0) : COLOR
{
	float4 texColor = tex2D(texSampler, uv);

	// uv.y 기준으로 게이지 잘라내기
	if (uv.x >= CutUV)
	{
		if (uv.x >= LerpUV)
		{
			//discard;
			return float4(0.5f, 0.5f, 0.5f, 0.5f);
		}
		return float4(0.9f, 0.9f, 0.9f, 1.f);
	}
	return texColor;
}
