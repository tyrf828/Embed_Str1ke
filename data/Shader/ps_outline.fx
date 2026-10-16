//----------------------------------------------------------------------------
//!	@file	ps_outline.fx
//!	@brief	輪郭線描画
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

// 定数バッファ
cbuffer OutlineColor : register(b12)
{
    float4 outline_color_;  //!< 輪郭線の色
};

// 頂点シェーダーの出力
struct VS_OUTPUT_MODEL
{
	float4	position_       : SV_Position;      //!< 座標       (スクリーン空間)
    float4  curr_position_  : CURR_POSITION;    //!< 現在の座標 (スクリーン空間)
	float3	world_position_ : WORLD_POSITION;   //!< ワールド座標
	float3	normal_         : NORMAL0;          //!< 法線
	float4	diffuse_        : COLOR0;           //!< Diffuseカラー
	float2	uv0_            : TEXCOORD0;        //!< テクスチャ座標
    float4  prev_position_  : PREV_POSITION;    //!< 1フレーム前の座標 (スクリーン空間)
};

typedef	VS_OUTPUT_MODEL	PS_INPUT_MODEL;

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT_MODEL input)
{
    PS_OUTPUT output;
    
    output.color0_ = outline_color_;
    
    //output.color0_.rgb = input.normal_;

    return output;
}
