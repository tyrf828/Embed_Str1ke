//----------------------------------------------------------------------------
//! @file   ps_lens_flare_ghost.fx
//! @brief  レンズフレア
//----------------------------------------------------------------------------
#include "dxlib_ps.h"

Texture2D TextureReduction0 : register(t30); // 1/4
Texture2D TextureReduction1 : register(t31); // 1/8

Texture2D TextureLensColor : register(t35); // レンズの色

//----------------------------------------------------------------------------
// ピクセルシェーダー
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    // https://john-chapman-graphics.blogspot.com/2013/02/pseudo-lens-flare.html
    PS_OUTPUT output;
    
    float2 uv = input.uv0_;
    
    // テクスチャカラーの読み込み
    float4 color = TextureReduction0.Sample(DiffuseSampler, uv);

   
    float ghost_dispersal = 1.3f;
    float2 ghost_dir = (float2(0.5f, 0.5f) - uv) * ghost_dispersal;
    
    
    float3 c = 0;
    for (int i = -2; i < 3; i++)
    {
        float2 sample_uv = (uv + ghost_dir * float(i));
        
        // 中心点から離れるにつれて暗くなる
        float weight = max(0, 0.499 - length(sample_uv - float2(0.5, 0.5))) * 2.0f;
        
        int index = i + 2;
        float u = float(index) * (1.0f / 5.0f);
        
        float3 lens_color = TextureLensColor.Sample(DiffuseSampler, float2(u, 0)).rgb;
        
        c += TextureReduction1.Sample(DiffuseSampler, sample_uv).rgb * weight * lens_color;

    }
    
    c *= 16.0f;
    
    output.color0_ = float4(c, 1.0f);
    
   
    return output;
}