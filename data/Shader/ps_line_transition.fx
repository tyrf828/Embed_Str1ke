//----------------------------------------------------------------------------
//!	@file	ps_line_transition.fx
//!	@brief	Line Transition Effect
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

// トランジション情報用定数バッファ
cbuffer TransitionInfo : register(b9)
{
    float transition_progress;   // トランジション進行度 (0.0～1.0)
    float blend_ratio;           // ノイズのブレンド率
    float2 padding; // パディング
};

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    float4 screen_color = DiffuseTexture.Sample(DiffuseSampler, input.uv0_);
    
    float base_mask = input.uv0_.x;

     float noise_uv = input.uv0_.y * 80;
    float noise_value = frac(sin(dot(noise_uv, float2(12.9898f, 78.233f))) * 43758.5453f);

    float blend_mask = lerp(base_mask, noise_value, blend_ratio);

    float final_mask = step(blend_mask, transition_progress);

    float4 black = float4(0.0, 0.0, 0.0, 1.0);
    
    float4 final = lerp(screen_color, black, final_mask);
    
    output.color0_ = final;
    
    return output;
}
