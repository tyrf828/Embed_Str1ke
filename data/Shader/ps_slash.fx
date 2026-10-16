//----------------------------------------------------------------------------
//! @file  ps_split_zoom.fx
//! @brief 画面を上下で分割エフェクト
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

// DxLibでは定数バッファは b4 以降が自由に利用できる
cbuffer SplitZoomParameter : register(b7)
{
    float  zoom_;       // 拡大率 (例: 2.0f)
    float3 padding_; // パディング
};

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    // 画面全体のUV
    float2 uv = input.uv0_;

    // 少しだけ斜めに分割する 
    // y = 0.5 + tilt * (x - 0.5))
    float tilt = 0.1f; // 少しだけ斜めに
    float boundary = 0.7f + tilt * (uv.x - 0.5f);

    // 上側 / 下側の中心位置
    float2 center = (uv.y < boundary) ? float2(0.25f, 0.25f) : float2(0.75f, 0.75f);

    // 中心からの距離を縮める事で拡大
    float2 src_uv = saturate(center + (uv - center) / zoom_);

    // サンプリング 
    float4 color = DiffuseTexture.Sample(DiffuseSampler, src_uv);

    // 出力カラー = テクスチャカラー * ディフューズカラー
    output.color0_ = color * input.diffuse_;
    return output;
}
