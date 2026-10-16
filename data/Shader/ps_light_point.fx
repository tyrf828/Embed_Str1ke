//----------------------------------------------------------------------------
//!	@file	ps_light_point.fx
//!	@brief	点光源
//----------------------------------------------------------------------------
#include "dxlib_ps.h"
#include "gbuffer.h.fx"
#include "lighting.h.fx"
#include "camera.h.fx"
#include "shadow.h.fx"

struct PS_OUTPUT_LIGHTING
{
    float4 diffuse_ : SV_Target0;
    float4 specular_ : SV_Target1;
};

//--------------------------------------------------------------
// 定数バッファ
//--------------------------------------------------------------
cbuffer PointLightInfo : register(b3)
{
    float3 light_position_; //!< 光源の位置座標
    float4 light_color_; //!< 光源のカラー
    float light_radius_; //!< 光源の影響範囲半径
    float light_intensity_; //!< 光源の強度
    float2 padding_;
};


//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT_LIGHTING main(PS_INPUT input)
{
    int2 position = input.position_.xy;
	
	// GBufferから情報を取得
    SurfaceInfo surface_info = GetSurfaceInfo(position);

    
    float3 L = light_position_ - surface_info.world_position_;
    float distance = length(L);

    // 球の半径の外側になった場合
    if (light_radius_ < distance)
    {
        discard;
    }


    L = normalize(L);
    float3 N = surface_info.normal_;
    float3 V = normalize(eye_position_ - surface_info.world_position_);
    float3 H = normalize(L + V);

    //----------------------------------------------------------
    // 光源計算
    //----------------------------------------------------------
    float3 diffuse;
    float3 specular;
	
    Lighting(light_color_.rgb * light_intensity_,
             N, L, V, H,
             surface_info.roughness_, surface_info.metallic_,
             surface_info.albedo_,
             diffuse, specular);

    // 距離減衰 https://lisyarus.github.io/blog/posts/point-light-attenuation.html
    {
        float s = saturate(distance / light_radius_);
        float f = 1.0f; // f 1.0~4.0 (大きくすると強く減衰する)
        float attenuation = (1 - s * s) * (1 - s * s) / (1 + f * s);

        diffuse *= attenuation;
        specular *= attenuation;
    }

    //----------------------------------------------------------
    // 出力
    //----------------------------------------------------------
    PS_OUTPUT_LIGHTING output;

    output.diffuse_ = float4(diffuse, 1.0f);
    output.specular_ = float4(specular, 1.0f);

	// 出力パラメータを返す
    return output;
}
