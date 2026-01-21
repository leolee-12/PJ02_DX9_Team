float4 fColor;

sampler2D texSampler : register(s0);

float4 PS_TintPixel(float2 uv : TEXCOORD0) : COLOR
{
	float4 texColor = tex2D(texSampler, uv);
	
	return texColor * fColor;
}
