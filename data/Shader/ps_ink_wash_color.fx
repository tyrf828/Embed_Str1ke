//----------------------------------------------------------------------------
//!	@file	ps_ink_wash_color.fx
//!	@brief	水墨色
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"


//----------------------------------------------------------------------------
//! @brief メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    float4 color = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);

    // グレースケール
    float gray = dot(color.rgb, float3(0.2125, 0.7154, 0.0721));

    // 元の色を白黒の色と混ぜる割合
    float saturation_ = 0.3;

    output.color0_ = float4(lerp(gray.xxx, color.rgb, saturation_), color.a);
    return output;
}
