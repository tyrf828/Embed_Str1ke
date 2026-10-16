//----------------------------------------------------------------------------
//!	@file	vs_outline.fx
//!	@brief	輪郭線描画頂点シェーダー
//----------------------------------------------------------------------------
#include "dxlib_vs_model.h"
#include "camera.h.fx"

// 定数バッファ
cbuffer OutlineParam : register(b12)
{
    float outline_width_; //!< 輪郭線の太さ
    float3 pos_; //!< 座標
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

};

//----------------------------------------------------------------------------
//	メイン関数
//----------------------------------------------------------------------------
VS_OUTPUT_MODEL main(VS_INPUT_MV1 input)
{
	// https://game.watch.impress.co.jp/docs/kikaku/1617901.html
    VS_OUTPUT_MODEL output;

    float3x4 mat_world = DxLib_WorldMatrix(input);

    float3 world_normal = normalize(mul(mat_world, float4(input.normal_, 0.0)));
    

	// ワールド空間へ変換
    float3 world_position = mul(mat_world, float4(input.position_, 1.0));
    
    float3 to_camera = normalize(eye_position_ - world_position);
            
    float3 dir = normalize(world_position - pos_);
    world_position += dir * outline_width_;
    
	// ビュー投影変換
    float3 view_position = mul(DxLib_ViewMatrix(), float4(world_position, 1.0));

    output.position_ = mul(DxLib_ProjectionMatrix(), float4(view_position, 1.0));

    output.world_position_ = world_position;

    output.curr_position_ = output.position_;

	//----------------------------------------------------------
	// 出力パラメータ
	//----------------------------------------------------------
    output.normal_ = world_normal;
    output.diffuse_ = input.diffuse_;
    output.uv0_ = input.uv0_.xy;
	
	//----------------------------------------------------------
	// 出力パラメータを返す
	//----------------------------------------------------------
    return output;
}
