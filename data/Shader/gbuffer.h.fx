//----------------------------------------------------------------------------
//!	@file	gbuffer.h.fx
//!	@brief	GBuffer関連
//----------------------------------------------------------------------------
#ifndef GBUFFER_H_FX
#define GBUFFER_H_FX

#include "dxlib_ps.h.fx"

//--------------------------------------------------------------
// Gbuffer
// gbuffer.hのフォーマット
//--------------------------------------------------------------
Texture2D GBuffer0    : register(t16);
Texture2D GBuffer1    : register(t17);
Texture2D GBuffer2    : register(t18);
Texture2D GBuffer3    : register(t19);
Texture2D DepthBuffer : register(t20);

// サーフェス情報
struct SurfaceInfo
{
    float3 albedo_;
    float  ao_;
    float3 normal_;
    float  roughness_;
    float  metallic_;
    float2 velocity_;
    float3 world_position_;
    float  depth_;
    float  emissive_;
    int    receives_decal_;
};

SurfaceInfo GetSurfaceInfo(int2 position)
{
    // GBufferを読み込む
    float4 gbuffer0 = GBuffer0.Load(int3(position, 0));
    float4 gbuffer1 = GBuffer1.Load(int3(position, 0));
    float4 gbuffer2 = GBuffer2.Load(int3(position, 0));
    float4 gbuffer3 = GBuffer3.Load(int3(position, 0));
    float  depth    = DepthBuffer.Load(int3(position, 0)).r;

    
    SurfaceInfo s;
    s.albedo_         = pow(abs(gbuffer0.rgb), 2.2); // リニア化
    s.ao_             = gbuffer0.a;
    s.normal_         = NormalDecode(gbuffer1.rg);
    s.roughness_      = gbuffer1.b;
    s.metallic_       = saturate(gbuffer1.a * (gbuffer1.a < 0.5f) ? 2.0f : 0.0f);
    s.velocity_       = gbuffer2.rg;
    s.world_position_ = gbuffer3.rgb;
    s.receives_decal_ = (gbuffer3.a > 0.5f) ? 1 : 0;
    s.depth_          = depth;
    s.emissive_       = saturate(gbuffer1.a * 2.0f - 1.0f) * 16.0f;


    return s;
}

#endif // GBUFFER_H_FX

