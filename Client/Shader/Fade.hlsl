
float Alpha;

sampler2D texSampler : register(s0);

float4 PS_Fade(float2 uv : TEXCOORD0) : COLOR
{
	float4 texColor = tex2D(texSampler, uv);

	
	return texColor * float4(1.f, 1.f, 1.f, Alpha);
}
