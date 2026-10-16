//----------------------------------------------------------------------------
//!	@file	ps_bloom.fx
//!	@brief	ブルーム合成
//----------------------------------------------------------------------------
#include "dxlib_ps.h"


Texture2D ReductionTexture0 : register(t30);
Texture2D ReductionTexture1 : register(t31);
Texture2D ReductionTexture2 : register(t32);
Texture2D ReductionTexture3 : register(t33);
Texture2D ReductionTexture4 : register(t34);

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;
    
    output.color0_ += ReductionTexture0.Sample(DiffuseSampler, input.uv0_);
    output.color0_ += ReductionTexture1.Sample(DiffuseSampler, input.uv0_);
    output.color0_ += ReductionTexture2.Sample(DiffuseSampler, input.uv0_);
    output.color0_ += ReductionTexture3.Sample(DiffuseSampler, input.uv0_);
    output.color0_ += ReductionTexture4.Sample(DiffuseSampler, input.uv0_);
    
	// 出力パラメータを返す
    return output;
}
