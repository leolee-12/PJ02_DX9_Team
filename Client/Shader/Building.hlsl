float fCanPlace; // 1.0이면 설치 가능, 0.0이면 불가능

sampler2D texSampler : register(s0);

float4 PS_Preview(float2 uv : TEXCOORD0) : COLOR
{
	float4 texColor = tex2D(texSampler, uv);

    // 반투명 처리
	if (texColor.a != 0.f)
	{
		texColor.a = 0.75f;
	}

	if (fCanPlace > 0.5f)
	{
        // 설치 가능 → 회색빛 틴트
		texColor.rgb *= float3(0.6f, 0.6f, 0.6f);
	}
	else
	{
        // 설치 불가능 → 붉은빛 틴트
		texColor.rgb *= float3(1.0f, 0.3f, 0.3f);
	}

	return texColor;
}

