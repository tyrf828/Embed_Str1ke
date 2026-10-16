//----------------------------------------------------------------------------
//!	@file	ps_light_directional.fx
//!	@brief	平行光源
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

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT_LIGHTING main(PS_INPUT input)
{
    int2 position = input.position_.xy;
	
	// GBufferから情報を取得
    SurfaceInfo surface_info = GetSurfaceInfo(position);

    float3 L = normalize(float3(1, 1, 1));
    float3 N = surface_info.normal_;
    float3 V = normalize(eye_position_ - surface_info.world_position_);
    float3 H = normalize(L + V);

    //----------------------------------------------------------
    // シャドウ
    //----------------------------------------------------------

    // 本来の3D奥行を再設定
    input.position_.w = length(surface_info.world_position_ - eye_position_);
    
    float shadow = GetShadow(input.position_, surface_info.world_position_);
    
    // Lambertと暗いほうを選択する
    shadow = min(shadow, saturate(dot(N, L)));

    
    //----------------------------------------------------------
    // 光源計算
    //----------------------------------------------------------
    float3 lightColor = float3(1, 1, 1) * 10;
	
    float3 diffuse;
    float3 specular;
	
    Lighting(lightColor,
             N, L, V, H,
             surface_info.roughness_, surface_info.metallic_,
             surface_info.albedo_,
             diffuse, specular);
    
    diffuse *= shadow;
    specular *= shadow;

    //----------------------------------------------------------
    // 出力
    //----------------------------------------------------------
    PS_OUTPUT_LIGHTING output;

    output.diffuse_ = float4(diffuse, 1.0f);
    output.specular_ = float4(specular, 1.0f);

	// 出力パラメータを返す
    return output;
}
