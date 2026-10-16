//===========================================================================
//! @file ps_scan_composite.fx
//! @brief スキャン波の合成
//===========================================================================
#include "dxlib_ps.h"
#include "camera.h.fx"

Texture2D WeaponDepth : register(t24);
Texture2D Depth : register(t25);
Texture2D WeaponMask : register(t26);

// デプスをビューのZ座標に変換する
float ViewDepth(float2 uv, float depth)
{
    float4 clip_pos = float4(uv * 2.0 - 1.0, depth, 1.0);
    clip_pos.y = -clip_pos.y;
    
    float4 view_pos = mul(clip_pos, mat_inv_proj_);
    view_pos /= view_pos.w;

    return view_pos.z;
}

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    float4 source = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);
    output.color0_ = source;
    int3 pixel = int3(int2(input.position_.xy), 0);
    float4 mask = WeaponMask.Load(pixel);
    
    if (mask.a <= 0.0f)
    {
        return output;
    }

    float depth = Depth.Load(pixel).r;
    float weapon_depth = WeaponDepth.Load(pixel).r;

    // 遮蔽物の後ろの武器だけ描画
    if (depth < 1.0f && ViewDepth(input.uv0_, weapon_depth) > ViewDepth(input.uv0_, depth) + 0.01f)
    {
        output.color0_.rgb = lerp(source.rgb, mask.rgb, saturate(mask.a));
    }
    return output;
}
