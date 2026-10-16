//----------------------------------------------------------------------------
//!	@file	ps_chromatic_aberration.fx
//!	@brief	色収差ピクセルシェーダー
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
	PS_OUTPUT	output;

    // 中心方向に向かうベクトル
    float2 dir = float2(0.5f, 0.5f) - input.uv0_;

    float2 distortion = dir * 0.1f;

	// テクスチャカラーの読み込み
    float r = DiffuseTexture.Sample(DiffuseSampler, input.uv0_ + distortion * 2.0f).r;
    float g = DiffuseTexture.Sample(DiffuseSampler, input.uv0_ + distortion).g;
    float b = DiffuseTexture.Sample(DiffuseSampler, input.uv0_).b;
    output.color0_ = float4(r, g, b, 1);

	// 出力パラメータを返す
	return output;
}
