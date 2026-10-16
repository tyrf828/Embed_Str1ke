//----------------------------------------------------------------------------
//!	@file	lighting.h.fx
//!	@brief	照明計算関連
//----------------------------------------------------------------------------
#ifndef LIGHTING_H_FX
#define LIGHTING_H_FX

#include "dxlib_ps.h.fx"

// GGX スペキュラーモデル (Trowbridge Reitz)
// エネルギー保存則(Normalized distribution function) / 微細表面(microfacet)
//!	@param	[in]	N			法線(Normal)
//!	@param	[in]	H			ハーフベクトル(Half)
//!	@param	[in]	roughness	表面の粗さ 0.0(つるつる)～1.0(ざらざら)
//!	@param	[in]	gamma		スペキュラーの減衰テールの係数
float D_GGX(float3 N, float3 H, float roughness)
{
	//                            a^2
	//	D(N, H, a) = -------------------------------------
	//                π * [ (a^2 - 1)( dot(N, H)^2 + 1]^2
	// ※a は roughness の2乗

    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;
    float numerator = a2;
    float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
    denominator = PI * denominator * denominator;
    return numerator / denominator;
}

//!	フレネル反射 (Schlick近似)
//!	[in]	f0		初期の反射率(fresnel bias)
//!	[in]	NdotV	NとVの内積値
float3 F_Schlick(float3 f0, float NdotV)
{
    float x = 1.0 - NdotV;
    return f0 + (1.0 - f0) * (x * x * x * x * x);
}

//! G項 Smith 自己遮蔽モデル
float G_Smith_Schlick_GGX(float roughness, float NdotV, float NdotL)
{
    float a = roughness * roughness;
    float k = a * 0.5f;
    float GV = NdotV / (NdotV * (1 - k) + k);
    float GL = NdotL / (NdotL * (1 - k) + k);

    return GV * GL;
}





void Lighting(  in float3 lightColor, //
                in float3 N, //
                in float3 L, //
                in float3 V, //
                in float3 H, //
                in float  roughness,
                in float  metallic,
                in float3 albedo,
                out float3 diffuse,
                out float3 specular)
{
    float NdotH = dot(N, H);
    float LdotH = dot(L, H);
    float NdotV = dot(N, V);
    float NdotL = saturate(dot(N, L));
    
    float reflectance = 0.04;
    float3 specularColor = lerp(float3(reflectance, reflectance, reflectance), albedo, metallic);
    
    
	//-------------------------------------------------------------
	// diffuse (ディフューズ) = 拡散反射光
	//-------------------------------------------------------------
	// Lambert 拡散反射光モデル
    // float diffuse = max(0, dot(N, L));
    const float Kd = 1.0 / PI;
    diffuse = lightColor * (NdotL * Kd) * albedo;

	//-------------------------------------------------------------
	// specular (スペキュラー) = 鏡面反射光
	//-------------------------------------------------------------
	// Cook-Torrance BRDF 近似高速化 (Optimizing PBR SIGGRAPH2015)
    float roughness4 = roughness * roughness * roughness * roughness;
    float denominator = (NdotH * NdotH * (roughness4 - 1.0) + 1.0) * LdotH;
	
    float3 brdf = roughness4 * rcp(4.0 * PI * denominator * denominator * (roughness + 0.5));
	
    specular = brdf * NdotL * specularColor * lightColor;
}

#endif // LIGHTING_H_FX

