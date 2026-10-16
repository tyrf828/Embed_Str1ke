//----------------------------------------------------------------------------
//! @file  ps_sphericalmask_inner.fx
//! @brief Spherical Mask Transition (inner)
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "camera.h.fx"

cbuffer CBufferMask : register(b6)
{
    float4 mask_world_position_; // マスク球の中心
    
    float mask_radius_; // マスク球の半径
    float mask_softness_; // 柔らかさ 
    float2 padding_;
};

// 頂点シェーダーの出力
struct VS_OUTPUT_MODEL
{
    float4 position_ : SV_Position; //!< 座標       (スクリーン空間)
    float4 curr_position_ : CURR_POSITION; //!< 現在の座標 (スクリーン空間)
    float3 world_position_ : WORLD_POSITION; //!< ワールド座標
    float3 normal_ : NORMAL0; //!< 法線
    float4 diffuse_ : COLOR0; //!< Diffuseカラー
    float2 uv0_ : TEXCOORD0; //!< テクスチャ座標
    float4 prev_position_ : PREV_POSITION; //!< 1フレーム前の座標 (スクリーン空間)
};

typedef VS_OUTPUT_MODEL PS_INPUT_MODEL;

struct PS_OUTPUT_MRT
{
    float4 color0_ : SV_Target0;
    float4 color1_ : SV_Target1;
    float4 color2_ : SV_Target2;
    float4 color3_ : SV_Target3;
};

PS_OUTPUT_MRT main(PS_INPUT_MODEL input)
{
    float2 uv = input.uv0_;
    float3 N = normalize(input.normal_);

    // 法線マップ
    N = Normalmap(N, input.world_position_, uv);

    // テクスチャカラー
    float4 texture_color = DiffuseTexture.Sample(DiffuseSampler, uv);
    texture_color = saturate(texture_color);

    // アルファテスト
    if (texture_color.a < 0.5)
    {
        discard;
    }
    
    float dist = distance(input.world_position_, mask_world_position_.xyz);

    // マスク値を計算
    float mask = smoothstep(
        mask_radius_ - mask_softness_,
        mask_radius_ + mask_softness_,
        dist
    );

    // 内側表示
    if (mask < 0.01)
    {
        discard;
    }
    
    // エッジ発光処理
    float dist_from_edge = abs(dist - mask_radius_); 
    float glow_intensity = 1.0 - smoothstep(0.0, mask_softness_, dist_from_edge);
    float3 final_color = texture_color.rgb + (float3(0.5f, 0.5f, 1.0f) * glow_intensity);
    texture_color.rgb = final_color;
    
    float3 albedo = texture_color.rgb;
    float roughness = 1.0;
    float metallic = 0.0;
    float ao = 1.0f;

    // 速度バッファ
    float2 velocity;
    {
        float4 curr = input.curr_position_;
        float4 prev = input.prev_position_;

        curr.xyz /= curr.w;
        prev.xyz /= prev.w;
    
        velocity = curr.xy - prev.xy;
        velocity = velocity * float2(0.5f, -0.5f);
    }
    
    // Emissive と Metallic をエンコード
    float4 dxlib_emissive = DxLib_Common.Material.Ambient_Emissive;
    float emissive = dot(dxlib_emissive.rgb, float3(0.299, 0.587, 0.114));
    emissive = saturate(emissive * (1.0f / 16.0f));
    float metel_emissive = (emissive < 0.0001f) ? saturate(metallic) * 0.5f : (saturate(emissive) * 0.5f + 0.5);

    PS_OUTPUT_MRT output;
    output.color0_ = float4(albedo, ao);
    output.color1_ = float4(NormalEncode(N).xy, roughness, metel_emissive);
    output.color2_ = float4(velocity.xy, 0, 0);
    output.color3_ = float4(input.world_position_.xyz, 1.0f);
    
    return output;
}
