//----------------------------------------------------------------------------
//!	@file	ps_model.fx
//!	@brief	MV1モデルピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "lighting.h.fx"
#include "shadow.h.fx"
#include "camera.h.fx"

//===========================================================================
// 頂点シェーダーの出力
//===========================================================================
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

//===========================================================================
// IBLテクスチャ
//===========================================================================
TextureCube TextureIBLDiffuse : register(t27);
TextureCube TextureIBLSpecular : register(t28);
Texture2D TextureIBLBrdf : register(t29);

//===========================================================================
// メイン関数
//===========================================================================
PS_OUTPUT main(PS_INPUT_MODEL input)
{
    PS_OUTPUT output;

    float2 uv = input.uv0_;
    float3 N = normalize(input.normal_);

    //------------------------------------------------------------
    // 法線マップ
    //------------------------------------------------------------
    N = Normalmap(N, input.world_position_, uv);

    //------------------------------------------------------------
    // テクスチャカラーを読み込み
    //------------------------------------------------------------
    float4 texture_color = DiffuseTexture.Sample(DiffuseSampler, uv);
    texture_color = saturate(texture_color);

    // アルファテスト
    if (texture_color.a < 0.5)
        discard;

    // sRGBへ変換
    float3 albedo = pow(abs(texture_color.rgb), 2.2);

    float roughness = 0.99;
    float metallic = 0.0;
    float ao = 1.0;

    float3 L = normalize(float3(1, 1, 1));

    float3 lightColor = float3(1, 1, 1) * 10;

    float3 V = normalize(eye_position_ - input.world_position_);
    float3 H = normalize(L + V);
    float3 R = normalize(reflect(-V, N));
    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));
    
    //----------------------------------------------------------
    // シャドウ
    //----------------------------------------------------------
    float4 screen_pos = input.position_;
    // 本来の3D奥行を再設定
    screen_pos.w = length(input.world_position_ - eye_position_);
    float shadow = GetShadow(screen_pos, input.world_position_);

    // Lambertと暗いほうを選択する
    shadow = min(shadow, saturate(dot(N, L)));


    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);

    //----------------------------------------------------------
    // 光源計算
    //----------------------------------------------------------
    Lighting(lightColor,
                 N, L, V, H,
                 roughness, metallic, albedo,
                 diffuse, specular);


    diffuse *= shadow;
    specular *= shadow;
    

    // IBL
    {
        float mip = saturate(roughness) * 7.0f;
        float3 ibl_diffuse = TextureIBLDiffuse.SampleLevel(DiffuseSampler, N, 0).rgb;
        float3 ibl_specular = TextureIBLSpecular.SampleLevel(DiffuseSampler, R, mip).rgb;

        float2 brdf = TextureIBLBrdf.SampleLevel(DiffuseSampler, float2(NdotV, 1.0f - roughness), 0).rg;

        float3 specular_color = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);

        ibl_diffuse *= (1.0f / PI) * albedo;
        ibl_specular *= specular_color * brdf.x + brdf.y;

        diffuse += ibl_diffuse;
        specular += ibl_specular;
    }

    float3 color_ao = albedo * ao;
    diffuse = lerp(diffuse * color_ao, diffuse, ao);

    float so = saturate(dot(N, V) + ao);
    so = saturate(so * so - 1.0f + ao);
    specular *= so;

    // 合成
    float3 final_color = diffuse + specular;
    output.color0_ = float4(final_color, DxLib_Base.factorColor_.a);
    return output;
}