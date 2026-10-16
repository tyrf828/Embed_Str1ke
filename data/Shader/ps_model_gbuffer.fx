//----------------------------------------------------------------------------
//!	@file	ps_model_gbuffer.fx
//!	@brief	MV1モデルピクセルシェーダー / GBuffer出力
//----------------------------------------------------------------------------
#include "dxlib_ps.h.fx"
#include "camera.h.fx"

// 頂点シェーダーの出力
struct VS_OUTPUT_MODEL
{
    float4 position_       : SV_Position;    //!< 座標       (スクリーン空間)
    float4 curr_position_  : CURR_POSITION;  //!< 現在の座標 (スクリーン空間)
    float3 world_position_ : WORLD_POSITION; //!< ワールド座標
    float3 normal_         : NORMAL0;        //!< 法線
    float4 diffuse_        : COLOR0;         //!< Diffuseカラー
    float2 uv0_            : TEXCOORD0;      //!< テクスチャ座標
    float4 prev_position_  : PREV_POSITION;  //!< 1フレーム前の座標 (スクリーン空間) 
};

typedef VS_OUTPUT_MODEL PS_INPUT_MODEL;

struct PS_OUTPUT_MRT
{
    float4 color0_ : SV_Target0;
    float4 color1_ : SV_Target1;
    float4 color2_ : SV_Target2;
    float4 color3_ : SV_Target3;
};

cbuffer ModelInfo : register(b11)
{
    int receives_decal;
    float3 padding;
};

//----------------------------------------------------------------------------
// メイン関数
//----------------------------------------------------------------------------
PS_OUTPUT_MRT main(PS_INPUT_MODEL input)
{
    float2 uv = input.uv0_;

    float3 N = normalize(input.normal_); // 法線
	
	//------------------------------------------------------------
	// 法線マップ
	//------------------------------------------------------------
    N = Normalmap(N, input.world_position_, uv);

	//------------------------------------------------------------
	// テクスチャカラーを読み込み
	//------------------------------------------------------------
    float4 texture_color = DiffuseTexture.Sample(DiffuseSampler, uv);
    texture_color = saturate(texture_color);
	
    // アルファテスト
    if (texture_color.a < 0.5)
        discard;
	
    float3 albedo = texture_color.rgb;
	
    float roughness = 1.0; // ラフ度 0.0:つるつる ～ 1.0:ざらざら 
    float metallic  = 0.0; // 金属度 0.0:非金属   ～ 1.0:金属     
    float ao = 1.0f;
    
    //----------------------------------------------------------
    // 出力
    //----------------------------------------------------------
    PS_OUTPUT_MRT output;

    // 速度バッファ
    float2 velocity;
    {
        float4 curr = input.curr_position_; // 現在の      スクリーン空間の座標(の頂点シェーダーの戻り値)
        float4 prev = input.prev_position_; // 1フレーム前のスクリーン空間の座標(の頂点シェーダーの戻り値)

        // 投影(wで割る) -> XY:スクリーン座標(-1～+1) Z:デプスバッファの値 W:ビュー座標系での奥行Z(m)
        curr.xyz /= curr.w;
        prev.xyz /= prev.w;
    
        // 移動量を差分計算(スクリーン座標系での移動量)
        velocity = curr.xy - prev.xy;
        
        // スクリーン座標系をテクスチャUV座標系に変換
        velocity = velocity * float2(0.5f, -0.5f);
    }
    
    //Emissive と Metallic をエンコード
    // 0.0~0.5 = Metallic / 0.5~1.0 = Emissive
    float4 dxlib_emissive = DxLib_Common.Material.Ambient_Emissive;
    
    float emissive = dot(dxlib_emissive.rgb , float3(0.299, 0.587, 0.114));
    // Emissiveは0~16までの値を想定
    emissive = saturate(emissive * (1.0f / 16.0f));
    
    float metel_emissive = (emissive < 0.0001f) ? saturate(metallic) * 0.5f : (saturate(emissive) * 0.5f + 0.5);
    
    output.color0_ = float4(albedo, ao);
    output.color1_ = float4(NormalEncode(N).xy, roughness, metel_emissive);
    output.color2_ = float4(velocity.xy , 0, 0);
    output.color3_ = float4(input.world_position_.xyz, receives_decal);
    	
	// 出力パラメータを返す
    return output;
}
