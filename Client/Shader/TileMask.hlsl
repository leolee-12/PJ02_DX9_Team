sampler2D texSampler : register(s0);

float4 PS_TileMask(float2 uv : TEXCOORD0) : COLOR
{
    float4 texColor = tex2D(texSampler, uv);
    
    if (texColor.a != 0.f)
    {
        return texColor * float4(0.f, 0.f, 0.f, 1.f);
    }
    return texColor;
}