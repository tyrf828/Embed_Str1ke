//---------------------------------------------------------------------------
//! @file   FilterVolumetricFog.cpp
//! @brief  ボリューメトリックフォグフィルター
//---------------------------------------------------------------------------
#include "FilterVolumetricFog.h"
#include <Game/Object/Shadowmap.h>
#include <Game/Object/GBuffer.h>

//! @brief 初期化
//! @return 初期化済み
bool FilterVolumetricFog::Init()
{
    SetName("FilterVolumetricFog");

    // ワークテクスチャを作成
    const int width  = WINDOW_W;
    const int height = WINDOW_H;
    texture_source_  = std::make_shared<Texture>(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

    // シェーダー初期化
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_volumetric_fog");
    buffer_cb_ = CreateShaderConstantBuffer(sizeof(FogInfo));

    // シャドウサンプラーを作成 (DirectX11)
    {
        ID3D11Device* d3dDevice = GetD3DDevice();

        D3D11_SAMPLER_DESC desc{
            .Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // ピクセルサンプリング動作モード
            .AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP, // テクスチャのアドレスモードU
            .AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP, // テクスチャのアドレスモードV
            .AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP, // テクスチャのアドレスモードW
            .MipLODBias     = 0.0f, // MIPレベルのバイアス値
            .MaxAnisotropy  = 1, // 異方性サンプリングの品質 [1,16]
            .ComparisonFunc = D3D11_COMPARISON_LESS, // 数値比較の場合の大小関係
            .BorderColor    = {1.0f, 1.0f, 1.0f, 1.0f}, // 範囲外の色(ボーダーカラー)
            .MinLOD         = -FLT_MAX, // 最小のLOD
            .MaxLOD         = +FLT_MAX, // 最大のLOD
        };

        d3dDevice->CreateSamplerState(&desc, &d3d_shadow_sampler_);
    }
    info_;
    auto execute = [this]() {
        if(!is_enable_)
        {
            return;
        }

        auto shadow = Scene::Object::Get<Shadowmap>();
        if(!shadow)
        {
            return;
        }

        // フォグ情報を定数バッファへ設定する
        auto* p = reinterpret_cast<FogInfo*>(GetBufferShaderConstantBuffer(buffer_cb_));
        *p      = info_;
        if(shadow)
        {
            float3 direction       = shadow->GetDirection();
            p->light_direction_[0] = direction.x;
            p->light_direction_[1] = direction.y;
            p->light_direction_[2] = direction.z;
        }
        UpdateShaderConstantBuffer(buffer_cb_);

        auto d3d_context = static_cast<ID3D11DeviceContext*>(const_cast<void*>(DxLib::GetUseDirect3D11DeviceContext()));

        // HDRのコピー
        CopyToRenderTarget(texture_source_.get(), GetHdrBuffer());

        SetTexture(24, shadow->GetShadowmapTexture());
        d3d_context->PSSetSamplers(14, 1, d3d_shadow_sampler_.GetAddressOf());
        SetShaderConstantBuffer(buffer_cb_, DX_SHADERTYPE_PIXEL, 13);
        // HDRへフォグを合成する
        CopyToRenderTarget(GetHdrBuffer(), texture_source_.get(), *shader_ps_);

        SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 13);
        SetTexture(24, nullptr);
    };

    SetProc("VolumetricFog", execute, ProcTiming::Filter, static_cast<ProcPriority>(1));

    return Super::Init();
}

//! @brief 終了
void FilterVolumetricFog::Exit()
{
    Super::Exit();
    if(buffer_cb_ >= 0)
    {
        DeleteShaderConstantBuffer(buffer_cb_);
    }
    buffer_cb_      = -1;
    texture_source_ = nullptr;
    shader_ps_      = nullptr;
    d3d_shadow_sampler_.Reset();
}

//! @brief GUI描画
void FilterVolumetricFog::GUI()
{
    Super::GUI();
    if(ImGui::Begin(GetName().data()))
    {
        ImGui::SliderFloat("Density", &info_.density_, 0.0f, 0.03f);
        ImGui::DragFloat("Base height", &info_.base_height_, 1.0f);
        ImGui::SliderFloat("Height falloff", &info_.height_falloff_, 0.0f, 0.1f);
        ImGui::SliderFloat("Anisotropy", &info_.anisotropy_, -0.1f, 0.1f);
        ImGui::SliderFloat("Light intensity", &info_.light_intensity_, 0.0f, 20.0f);
    }
    ImGui::End();
}
