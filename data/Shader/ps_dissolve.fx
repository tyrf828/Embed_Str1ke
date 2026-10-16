//----------------------------------------------------------------------------
//!	@file	ps_dissolve.fx
//!	@brief	Dissolve ピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

cbuffer DissolveInfo : register(b8)
{
    float dissolve_amount_; // Dissolve量
    float3 padding_;
};

Texture2D NoiseTexture : register(t11); // ノイズテクスチャ

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

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_MODEL input)
{
    PS_OUTPUT output;

    float2 uv = input.uv0_;
    float3 N = normalize(input.normal_); // 法線

	//------------------------------------------------------------
	// 法線マップ
	//------------------------------------------------------------
    N = Normalmap(N, input.world_position_, uv);

	//------------------------------------------------------------
	// テクスチャカラーを読み込み
	//------------------------------------------------------------
    float4 texture_color = DiffuseTexture.Sample(DiffuseSampler, uv);

    //------------------------------------------------------------
    // Dissolve処理
    //------------------------------------------------------------
    // ノイズテクスチャからdissolve値を取得
    float noise = NoiseTexture.Sample(DiffuseSampler, uv).r;
    
    float dissolve_value = noise - dissolve_amount_;
    
    if (dissolve_value < 0.0f)
    {
        discard;
    }
    
    // エッジ発光処理
    float edge_factor = smoothstep(0.0f, 0.1f, dissolve_value);
    float3 final_color = lerp(float3(0.0f, 1.0f, 1.0f), texture_color.rgb, edge_factor);
    
    output.color0_.rgb = final_color;
    output.color0_.a = texture_color.a;
	
	// 出力パラメータを返す
    return output;
}
