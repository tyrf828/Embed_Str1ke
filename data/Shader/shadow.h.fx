//----------------------------------------------------------------------------
//!	@file	shadow.h.fx
//!	@brief	シャドウ関連
//----------------------------------------------------------------------------
#ifndef SHADOW_H_FX
#define SHADOW_H_FX

#include "camera.h.fx"
#include "dxlib_ps.h.fx"

Texture2D ShadowmapTexture : register(t13);
// シャドウデプス比較専用サンプラー
SamplerComparisonState ShadowmapSampler : register(s13);

// 0.0～1.0の範囲でランダムな値を生成
float InterleavedGradientNoise(float2 position_screen)
{
    float3 magic = float3(0.06711056f, 0.00583715f, 52.9829189f);
    return frac(magic.z * frac(dot(position_screen, magic.xy)));
}


//---------------------------------------------------------------------------
// Vogel disk sampling
//---------------------------------------------------------------------------
float2 VogelDiskSample(int index, int num_samples, float angle_bias = 0.0f)
{
	// Vogel disk sampling
	// https://www.shadertoy.com/view/4l3yRM
	
    const float GOLDEN_ANGLE = 2.399963229728653; // 黄金角 (黄金比に基づく角度)
	
    float theta = GOLDEN_ANGLE * float(index) + angle_bias;

    float r = sqrt((float(index) + 0.5) / float(num_samples));
    return sin(float2(theta + (PI / 2.0), theta)) * r;
}


float GetShadow(float4 input_position, float3 world_position)
{
    //----------------------------------------------------------
   	// カスケード番号を選択する
    //----------------------------------------------------------
    int cascade_index = 0;
	{
        static const float split_distance[4] =
        {
            75.0f, 200.0f, 1000.0f, 1800.0f
        };
		
        float distance = input_position.w; // カメラからの距離(m)
  
        if (split_distance[0] < distance)
            cascade_index++;
        if (split_distance[1] < distance)
            cascade_index++;
        if (split_distance[2] < distance)
            cascade_index++;
    }
    
    //----------------------------------------------------------
   	// ソフトシャドウサンプリング PCF(Percentage Closer Filtering)
    //----------------------------------------------------------
    float shadow = 1.0f; // シャドウの初期値 (1.0:影なし, 0.0:影あり)
	{
        float4 screen_position = mul(float4(world_position, 1.0f), light_view_proj_[cascade_index]);
        screen_position.xyz /= screen_position.w; // 正規化

        float2 shadow_uv = screen_position.xy * float2(0.5f, -0.5f) + 0.5f;
 
		
        shadow_uv.x = shadow_uv.x * 0.25f + 0.25f * cascade_index;
		
		
        shadow = 0.0f;

        float theta_bias = InterleavedGradientNoise(input_position.xy) * (2.0f * PI);

		
        const int SAMPLE_NUM = 16; // サンプリング数 
        [unroll]
        for (int i = 0; i < SAMPLE_NUM; i++)
        {
            float2 offset = VogelDiskSample(i, SAMPLE_NUM, theta_bias) * (1.0 / 512.0);
		
			// シャドウマップの深度値と比較
            static const float shadow_bias = 0.0001f; // シャドウバイアス (深度値のオフセット)

            shadow += ShadowmapTexture.SampleCmp(ShadowmapSampler, shadow_uv + offset * float2(0.25f, 1.0f), screen_position.z - shadow_bias).r;
        }
		
        shadow *= 1.0f / SAMPLE_NUM;
    }
    return shadow;
}

#endif // SHADOW_H_FX

