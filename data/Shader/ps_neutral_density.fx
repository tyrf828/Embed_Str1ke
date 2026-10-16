//----------------------------------------------------------------------------
//!	@file	ps_neutral_density.fx
//!	@brief	減光フィルター
//----------------------------------------------------------------------------
#include "dxlib_ps.h"
#include "gbuffer.h.fx"

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    
    int2 position = input.position_.xy;
    // GBufferから情報を取得
    SurfaceInfo surface_info = GetSurfaceInfo(position);	
    
    output.color0_ = DiffuseTexture.Sample(DiffuseSampler, input.uv0_) * 0.005f;
	// 出力パラメータを返す
    return output;
}
