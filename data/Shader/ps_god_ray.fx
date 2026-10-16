//----------------------------------------------------------------------------
//!	@file	ps_god_ray.fx
//!	@brief	ゴッドレイ
//----------------------------------------------------------------------------
#include "dxlib_ps.h"
#include "gbuffer.h.fx"
#include "shadow.h.fx"

cbuffer GodRayInfo : register(b13)
{
    float2 light_pos_; // 画面上の光源位置
    float2 padding_;
};

//----------------------------------------------------------------------------
//! @brief メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    
    float density = 64.0f; // サンプリング間隔

    float threshold = 0.7f; // 光として使う境界線
    
    float depth_softness = 0.1f; // 柔らかさ

    // 現在の画素から光源方向へ向かってサンプリング
    float2 delta = (light_pos_ - input.uv0_) / density;
    
    // 光の輪郭を滑らかにする
    float noise_value = InterleavedGradientNoise(input.position_.xy);

    float2 sample_uv = input.uv0_ + delta * noise_value;
    
    float decay = 0.93f; // 減衰率
    float decay_ratio = 1.0f; // 減衰率の累積値       
    float intensity = 0.03f; // 光の強さ
    
    // 遮蔽物に当たったか判定値
    float ray_visibility = 1.0f;

    
    float3 color = 0.0f;

    [unroll]
    for (int i = 0; i < (int) density; i++)
    {
        sample_uv += delta;
        
        float2 inside = step(0.0f, sample_uv) * step(sample_uv, 1.0f);
        float in_screen = inside.x * inside.y;
        ray_visibility *= in_screen;

        float3 sample_color = DiffuseTexture.Sample(DiffuseSampler, sample_uv).rgb;
        
        // 明るい画素だけ抽出
        float luminance = dot(sample_color, float3(0.299f, 0.587f, 0.114f));
        float mask = saturate((luminance - threshold) / max(1.0f - threshold, 0.001f));

        float sample_depth = DepthBuffer.Sample(DiffuseSampler, sample_uv).r;
        
        // 遮蔽物に当たったか判定
        float visibility = smoothstep(0.9f - depth_softness, 0.9f + depth_softness, sample_depth);

        ray_visibility *= visibility;
        
        color += sample_color * mask * decay_ratio * intensity * ray_visibility;

        decay_ratio *= decay;
    }

    output.color0_ = float4(color, 1.0f);
    return output;
}
