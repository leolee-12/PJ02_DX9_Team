// 게이지 비율 (0.0 ~ 1.0)
float gaugeRatio;
float4 fGaugeRGBA;

sampler2D texSampler : register(s0);

float4 PS_Gauge(float2 uv : TEXCOORD0) : COLOR
{
    float4 texColor = tex2D(texSampler, uv);
    
    // uv.y 기준으로 게이지 잘라내기
    if (uv.y < 1.0 - gaugeRatio)
        discard; // 이 픽셀은 출력하지 않음

    // 정상적으로 텍스처 출력
    return texColor * fGaugeRGBA;
}