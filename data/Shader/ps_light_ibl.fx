//----------------------------------------------------------------------------
//!	@file	ps_light_ibl.fx
//!	@brief	Image Based Lighting
//----------------------------------------------------------------------------
#include "dxlib_ps.h"
#include "gbuffer.h.fx"
#include "camera.h.fx"

struct PS_OUTPUT_LIGHTING
{
    float4 diffuse_  : SV_Target0;
    float4 specular_ : SV_Target1;
};

// Diffuse
TextureCube TextureIBLDiffuse : register(t27);
// Specular
TextureCube TextureIBLSpecular : register(t28);
// Brdf
Texture2D   TextureIBLBrdf : register(t29);


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

    float NdotV = saturate(dot(N, V));
    
    
    // 反射ベクトル
    float3 R = reflect(-V, N);      // -V = 目線ベクトル

    // IBLスペキュラー内のラフネスピクセルはmipに格納されている
    float mip = surface_info.roughness_ * 7;
    
    //----------------------------------------------------------
    // 光源計算
    //----------------------------------------------------------
    float3 diffuse  = TextureIBLDiffuse.SampleLevel(DiffuseSampler, N, 0).rgb;
    float3 specular = TextureIBLSpecular.SampleLevel(DiffuseSampler, R, mip).rgb;
    float2 brdf = TextureIBLBrdf.SampleLevel(DiffuseSampler, float2(NdotV, 1.0 - surface_info.roughness_), 0).rg;

    float3 specular_color = lerp(float3(0.04, 0.04, 0.04), surface_info.albedo_.rgb, surface_info.metallic_);
 
    float scale = 1.0f;
    
    diffuse  *= (1.0f / PI)   * surface_info.albedo_ * scale;
    specular *= specular_color * brdf.x + brdf.y;
    
    //----------------------------------------------------------
    // 出力
    //----------------------------------------------------------
    PS_OUTPUT_LIGHTING output;

    output.diffuse_  = float4(diffuse , 1.0f);
    output.specular_ = float4(specular, 1.0f);

	// 出力パラメータを返す
	return output;
}
