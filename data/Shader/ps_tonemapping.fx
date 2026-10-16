//----------------------------------------------------------------------------
//!	@file	ps_tonemapping.fx
//!	@brief	HDRトーンマッピング
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

//https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
static const float3x3 ACESInputMat =
{
    { 0.59719, 0.35458, 0.04823 },
    { 0.07600, 0.90834, 0.01566 },
    { 0.02840, 0.13383, 0.83777 }
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367 },
    { -0.10208, 1.10813, -0.00605 },
    { -0.00327, -0.07276, 1.07602 }
};

// RRTとODTを合わせた関数
float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 ACESFitted(float3 color)
{
    color *= 0.8f;
    color = mul(ACESInputMat, color);

    color = RRTAndODTFit(color);

    color = mul(ACESOutputMat, color);

    color = saturate(color);

    return color;
}

// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
// ACES高速化関数
float3 ACESFilm(float3 x)
{
    x *= 0.5;
	
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

	// テクスチャカラーの読み込み
    float4 color = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);

	// トーンマッピング適用
    color.rgb = ACESFitted(color.rgb);

	// sRGBへ変換
    color.rgb = pow(color.rgb, 1.0 / 2.2);

    output.color0_ = color;

	// 出力パラメータを返す
    return output;
}
