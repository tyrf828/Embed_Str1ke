//----------------------------------------------------------------------------
//! @file   ps_volumetric_fog.fx
//! @brief  ボリューメトリックフォグ
//----------------------------------------------------------------------------
#include "dxlib_ps.h"
#include "camera.h.fx"
#include "shadow.h.fx"
#include "gbuffer.h.fx"

Texture2D FogShadowAtlas : register(t24);
SamplerComparisonState FogShadowSampler : register(s14);

cbuffer VolumetricFogInfo : register(b13)
{
    float density_; //!< 霧の密度
    float base_height_; //!< ベースの高さ
    float height_falloff_; //!< 高さの減衰率
    float anisotropy_; //!< 散乱方向(0~1)

    float light_intensity_; //!< 光の強さ
    float3 light_direction_; //!< 光源へ向かう方向
};


float GetShadow(float3 world_position, float view_depth, Texture2D shadow_texture, SamplerComparisonState shadow_sampler)
{
    
    //----------------------------------------------------------
   	// カスケード番号を選択する
    //----------------------------------------------------------
    int cascade_index = 0;
	{
        static const float split_distance[4] =
        {
            75.0f, 200.0f, 1000.0f, 1800.0f
        };
		
        float distance = view_depth; // カメラからの距離(m)
  
        if (split_distance[0] < distance)
            cascade_index++;
        if (split_distance[1] < distance)
            cascade_index++;
        if (split_distance[2] < distance)
            cascade_index++;
    }

    // 光源カメラへ投影し、アトラスのUVへ変換
    float4 screen_position = mul(float4(world_position, 1.0f), light_view_proj_[cascade_index]);
    screen_position.xyz /= screen_position.w;
    float2 shadow_uv = screen_position.xy * float2(0.5f, -0.5f) + 0.5f;
    shadow_uv.x = (shadow_uv.x + cascade_index) * 0.25f;
    
    static const float shadow_bias = 0.0001f; // シャドウバイアス (深度値のオフセット)

    return shadow_texture.SampleCmpLevelZero(shadow_sampler, shadow_uv, screen_position.z - shadow_bias).r;
    
}

//! @brief デプスからワールド座標へ変換
//! @param uv UV座標
//! @param depth デプスバッファ
//! @return ワールド座標
float3 WorldPosition(float2 uv, float depth)
{
    float4 clip_pos = float4(uv * 2.0 - 1.0, depth, 1.0);
    clip_pos.y = -clip_pos.y;
    
    float4 view_pos = mul(clip_pos, mat_inv_proj_);
    view_pos /= view_pos.w;
    
    float4 world_pos = mul(view_pos, mat_inv_view_);
    return world_pos.xyz;
}


//! @brief Henyey-Greenstein
//! @return 散乱の強さ
float HenyeyGreenstein(float g, float cos)
{
    float g2 = g * g;
    float denominator = max(1.0 + g2 - 2.0 * g * clamp(cos, -1.0f, 1.0f), 0.0001f);
    return (1.0 - g2) / (4.0 * PI * pow(denominator, 1.5f));
}

//! @brief Beer-Lambertの法則
//! @param distance 光が通過する距離
//! @return 透過率
float BeerLambert(float distance)
{
    return exp(-distance);
}

//----------------------------------------------------------------------------
//! @brief メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    
    int2 position = input.position_.xy;
    float2 uv = input.uv0_;

    SurfaceInfo surface = GetSurfaceInfo(position);

    // レイを視点からサンプル位置までの方向ベクトルに変換
    float3 start = WorldPosition(uv, 0.0f);
    float3 end = surface.depth_ < 1.0f ? surface.world_position_ : WorldPosition(uv, surface.depth_);
    
    float3 segment = end - start;
    float distance = length(segment);
    
    float3 direction = segment / max(distance, 0.0001f);
    
    // サンプルの間隔距離
    const int sample_count = 48;
    float sample_length = distance / sample_count;
    
    
    // 散乱の偏りを計算
    float phase = HenyeyGreenstein(anisotropy_, dot(direction, normalize(light_direction_)));
    
    float transmittance = 1.0f; //!< 視点からサンプル位置までの透過率
    float3 scattering = 0.0f; //!< 視点からサンプル位置までの光の累積
    
    float3 light_color = float3(1.0f, 1.0f, 1.0f);


    [loop]
    for (int i = 0; i < sample_count; i++)
    {
        float3 step_position = start + direction * ((i + 0.5f) * sample_length);

        float height = exp(-(step_position.y - base_height_) * height_falloff_);
        
        // 光学の厚さ = 濃度 × 高さ × 長さ
        float optical_depth = density_ * height * sample_length;
        // 透過率
        float step_transmittance = BeerLambert(optical_depth);
        // 不透明度
        float opacity = 1.0f - step_transmittance;
        // ビュー空間の距離
        float view_depth = mul(float4(step_position, 1.0f), mat_view_).z;
        
        float shadow = GetShadow(step_position, view_depth, FogShadowAtlas, FogShadowSampler);

        // 入射光
        float3 incident = light_color * light_intensity_ * phase * shadow;
        // 距離により散乱光の累積値を更新
        scattering += transmittance * opacity * incident;
        // 光がどれだけ残っているかを更新
        transmittance *= step_transmittance;
        
        if (transmittance < 0.001f)
        {
            break;
        }
    }
    
    float4 diffuse = DiffuseTexture.Sample(DiffuseSampler, uv);
    
    output.color0_ = float4(diffuse.rgb * transmittance + scattering, diffuse.a);
    return output;
}
