//----------------------------------------------------------------------------
//!	@file	ps_ink_wash_color.fx
//!	@brief	水墨シェーダー
//----------------------------------------------------------------------------
#include "gbuffer.h.fx"
#include "camera.h.fx"

cbuffer InkWashInfo : register(b13)
{
    float2 uv_pixel_;
    float2 padding1_;
    float edge_width_;
    float sensitivity_;
    float noise_scale_;
    float padding2_;
};

Texture2D NoiseTexture : register(t24);
SamplerState NoiseSampler : register(s14);

// グレースケール
float Gray(float3 color)
{
    return dot(color, float3(0.2125, 0.7154, 0.0721));
}

// 法線で輪郭を検出
float3 EdgeNormal(float2 uv)
{
    int2 p = clamp(int2(uv / uv_pixel_), int2(0, 0), resolution_ - 1);
    SurfaceInfo s = GetSurfaceInfo(p);
    
    float3 normal = float3(0.0, 0.0, 0.0);
    
    if (s.depth_ < 1.0)
    {
        normal = s.normal_;
    }
    return normal;
}


// 輪郭検出
float DetectEdge(float2 uv)
{
    float2 offset = uv_pixel_ * edge_width_;
    // 上下左右の法線差
    float3 d0 = EdgeNormal(uv + offset) - EdgeNormal(uv - offset);
    // 対角線の法線差
    float3 d1 = EdgeNormal(uv + offset * float2(1, -1)) - EdgeNormal(uv + offset * float2(-1, 1));

    float difference = Gray(d0 * d0 + d1 * d1);
    
    float edge = pow(difference, 0.2);

    return smoothstep(sensitivity_, sensitivity_ + 0.15, edge);
}

// ノイズ
float BrushNoise(float2 uv)
{
    float2 offset = float2(0.01, 0.0);
    float noise = NoiseTexture.Sample(NoiseSampler, uv).r * 4.0;
    noise += NoiseTexture.Sample(NoiseSampler, uv + offset.xy).r;
    noise += NoiseTexture.Sample(NoiseSampler, uv - offset.xy).r;
    noise += NoiseTexture.Sample(NoiseSampler, uv + offset.yx).r;
    noise += NoiseTexture.Sample(NoiseSampler, uv - offset.yx).r;
    return noise / 16.0;
}

//----------------------------------------------------------------------------
//! @brief メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    // https://zhuanlan.zhihu.com/p/34406208
    // https://zhuanlan.zhihu.com/p/63893540
    PS_OUTPUT output;

    float2 uv = input.uv0_;

    float3 diffuse = DiffuseTexture.Sample(DiffuseSampler, uv).rgb;
    float edge = DetectEdge(uv);

    int2 pixel = int2(input.position_.xy);
    
    SurfaceInfo surface = GetSurfaceInfo(pixel);
    
    float noise = 0.5;
    if (surface.depth_ < 1.0f)
    {
        noise = BrushNoise(uv);
    }

    float brightness = 1.0 + 0.1 * (noise - 0.5);

    float edge_amount = edge * (0.5 + 0.5 * noise);
    
    float3 edge_color_ = float3(0.015, 0.012, 0.01);
    float3 ink = lerp(diffuse * brightness, edge_color_, edge_amount);

    output.color0_ = float4(ink, 1.0);
    return output;
}
