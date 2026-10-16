//----------------------------------------------------------------------------
//!	@file	ps_light_finish.fx
//!	@brief	照明パスの最終合成
//----------------------------------------------------------------------------
#include "dxlib_ps.h"
#include "gbuffer.h.fx"
#include "camera.h.fx"

Texture2D LightDiffuseTexture : register(t21); // Diffuseテクスチャ
Texture2D LightSpecularTexture : register(t22); // Specularテクスチャ

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    int2 position = input.position_.xy; // ピクセル位置
	
	// GBufferから情報を取得
    SurfaceInfo surface_info = GetSurfaceInfo(position);
	
    PS_OUTPUT output;
    output.color0_ = 0.0;

    // 無限遠はAlbedoをそのまま使う 
    if (surface_info.depth_ == 1.0f)
    {
        output.color0_.rgb = surface_info.albedo_ * 2.0f;
        return output;
    }
	
    float4 diffuse = LightDiffuseTexture.Load(int3(position, 0));
    float4 specular = LightSpecularTexture.Load(int3(position, 0));
	
    float ao = surface_info.ao_ * surface_info.ao_;
    
    
    float so;
    {   
        float3 N = surface_info.normal_;
        float3 V = normalize(eye_position_ - surface_info.world_position_);
        so = dot(N, V) + ao;
        so = saturate(so * so - 1.0f + ao);
    }
    {
    float3 color_ao = surface_info.albedo_ * ao;
    diffuse.rgb = lerp(diffuse.rgb * color_ao, diffuse.rgb, ao);
    }
    
    output.color0_ = diffuse + specular * so;

    // Emissive
    output.color0_.rgb += surface_info.albedo_ * surface_info.emissive_;
    
    
	// 出力パラメータを返す
    return output;
}
