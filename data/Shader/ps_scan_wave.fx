//===========================================================================
//! @file ps_scan_wave.fx
//! @brief スキャン波
//===========================================================================
#include "dxlib_ps.h"
#include "gbuffer.h.fx"


cbuffer ScanInfo : register(b13)
{
    float3 world_position; //!< 座標
    float padding1; //!< パディング
    float3 color; //!< 半径
    float padding2; //!< パディング
    float radius; //!< 色
    float width; //!< 幅
    float intensity; //!< 光の強さ

};

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    float4 source = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);
    output.color0_ = source;

    SurfaceInfo surface = GetSurfaceInfo(int2(input.position_.xy));

    if (surface.depth_ >= 1.0f)
    {
        return output;
    }

    // スキャン波の中心からの距離を計算
    float distance = length(surface.world_position_ - world_position);
    
    // 色を加算
    float diff = radius - distance;
    float band = 0.0f;
    if (diff >= 0.0f)
    {
        band = 1.0f - smoothstep(0.0f, width, diff);
        output.color0_.rgb += color * band * intensity;
    }
    return output;
}
