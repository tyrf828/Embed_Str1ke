//----------------------------------------------------------------------------
//!	@file	ps_ssao.fx
//!	@brief	Screen Space Ambient Occlusion
//----------------------------------------------------------------------------
#include "dxlib_ps.h"
#include "gbuffer.h.fx"
#include "lighting.h.fx"
#include "camera.h.fx"
#include "shadow.h.fx"

// 0.0～1.0の範囲でランダムな値を生成
float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898f, 78.233f))) * 43758.5453f);
}

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target0
{
    int2 position = input.position_.xy;
	
	// GBufferから情報を取得
    SurfaceInfo surface_info = GetSurfaceInfo(position);

    float3 N = surface_info.normal_;
    float3 V = normalize(eye_position_ - surface_info.world_position_);

    // このシェーダー内はプリミティブは2D全体描画のため、本来の3D奥行を再設定
    input.position_.w = length(surface_info.world_position_ - eye_position_);
  
    //----------------------------------------------------------
    // SSAO
    //----------------------------------------------------------
    float ao = 0.0f;
    

    // 半球状の基準ベクトルを計算 (vN vT vB)  
    float3 vN = N; // 半球の上方向ベクトル

    float cosine = abs(dot(vN, float3(0, 1, 0)));

    float3 vT = cross(vN, cosine < 0.7 ? float3(0, 1, 0) : float3(1, 0, 0));
    float3 vB = cross(vT, vN);
    vT = normalize(vT); // 横方向のベクトル1
    vB = normalize(vB); // 横方向のベクトル2
    
    // らせんサンプリングの開始角度 (ピクセル毎にランダム)
    float base_angle = rand(input.position_.xy * 0.001) * 2.0f * PI * delta_time_;
    
    float3 view_center_position = mul(float4(surface_info.world_position_.xyz, 1), mat_view_).xyz;
    
    // 半球状にサンプリング
    static const int SAMPLE_COUNT = 16;
    [unroll]
    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        // らせんサンプリング
        float2 offset = VogelDiskSample(i, SAMPLE_COUNT, base_angle);
        float height = rand(input.uv0_ + i);
        
        float3 dir = normalize(float3(offset.xy, height));
        
        dir *= 2.0f;
        
        float3 offset_3d = (vT * dir.x) + (vB * dir.y) + (vN * dir.z);
        
        float3 sample_position = surface_info.world_position_ + offset_3d;

        //------------------------------------------------------
        // スクリーン座標の位置を求める
        //------------------------------------------------------
        float3 view_position = mul(float4(sample_position, 1), mat_view_).xyz;
        float4 screen_position = mul(float4(view_position, 1), mat_proj_);
        
        screen_position.xyz /= screen_position.w;

        // サンプリング位置のUV座標を求める        
        float2 uv = screen_position.xy * float2(0.5, -0.5) + 0.5;
        uv = saturate(uv);
        
        // デプスバッファを読み込み
        float2 resolution = float2(resolution_.x - 1, resolution_.y - 1);
        SurfaceInfo depth_surface = GetSurfaceInfo(int2(uv * resolution));

        // デプスバッファの値からビュー空間のxyzを求める
        float4 view_depth = mul(float4(screen_position.xy, depth_surface.depth_, 1), mat_inv_proj_);
        view_depth.xyz /= view_depth.w;
        
        // 遮蔽された場合
        if (view_depth.z < view_position.z)
        {
            // 角度から遮蔽量を計算する
            float3 dir = normalize(view_depth.xyz - view_center_position);
            float3 horizon = normalize(view_position - view_center_position);
            
            float cosine = dot(dir, horizon); // 1.0:遮蔽なし 0.0:完全遮蔽
            
            float ao_value = cosine / SAMPLE_COUNT;
            
            float distance = abs(view_depth.z - view_position.z);
            ao_value = lerp(ao_value, 0.0f, saturate(distance - 1.0f));
            
            ao += ao_value;
            
        }
    }
    ao *= 1.0f - smoothstep(100.0f, 200.0f, view_center_position.z);
    
            
    ao = 1 - saturate(ao);
    
    // AlbedoのRGBには影響を与えずA値のaoのみを出力
    return float4(1.0f, 1.0f, 1.0f, ao);
}
