//----------------------------------------------------------------------------
//!	@file	ps_decal.fx
//!	@brief	 デカールピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"
#include "gbuffer.h.fx"
#include "camera.h.fx"
#include "shadow.h.fx"
#include "ps_model_gbuffer.fx"

cbuffer DecalInfo : register(b5)
{
    matrix mat_inverse_world_; //!< ワールド逆行列
    float alpha_;             //!< 透明度
    float3 padding_;          //!< パディング
};

// テクスチャ
Texture2D DecalTexture : register(t36);


//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    SurfaceInfo surface_info = GetSurfaceInfo(input.position_.xy);

    float2 screen_uv = input.position_.xy / resolution_;

    if (surface_info.receives_decal_ == 0)
    {
        // デカールを受け取るオブジェクトでない場合は、そのまま出力
        discard;
    }

    float4 clip_pos;
    clip_pos.x = screen_uv.x * 2.0f - 1.0f;
    clip_pos.y = (1.0f - screen_uv.y) * 2.0f - 1.0f;
    clip_pos.z = surface_info.depth_;
    clip_pos.w = 1.0f;

    // ワールド座標に変換
    float4 view = mul(clip_pos, mat_inv_proj_);
    view /= view.w;
    float4 world_pos = mul(view, mat_inv_view_);
    float3 local_position = mul(world_pos, mat_inverse_world_).xyz;



    clip(0.5f - abs(local_position.xyz));

    float2 decal_uv = local_position.xz + 0.5f;
    float4 color = DecalTexture.Sample(DiffuseSampler, decal_uv);

    //----------------------------------------------------------
    // シャドウ
    //----------------------------------------------------------
    float3 L = normalize(float3(1, 1, 1));

    // 本来の3D奥行を再設定
    input.position_.w = length(surface_info.world_position_ - eye_position_);
    
    float shadow = GetShadow(input.position_, surface_info.world_position_);
    
    // Lambertと暗いほうを選択する
    shadow = min(shadow, saturate(dot(surface_info.normal_, L)));

    // 照明の影響が入っていないため、擬似的に影を薄くする
    shadow = shadow * 0.5 + 0.5;


    PS_OUTPUT output;
    output.color0_ = color * shadow;
    output.color0_.a = color.a * alpha_;

    return output;
}
